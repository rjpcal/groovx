///////////////////////////////////////////////////////////////////////
//
// tclveccmds.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 12:16:22 1999
// written: Thu Mar 16 09:59:25 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMDS_CC_DEFINED
#define TCLVECCMDS_CC_DEFINED

#include "tcl/tclveccmds.h"

#include "tcl/tclitempkgbase.h"
#include "util/strings.h"

#include <vector>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TVecGetterCmd definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::VecGetterBaseCmd::VecGetterBaseCmd(TclItemPkgBase* pkg, const char* cmd_name,
													 const char* usage, int item_argn) :
  TclCmd(pkg->interp(), cmd_name,
			usage ? usage : (item_argn ? "item_id(s)" : NULL), 
			item_argn+1, item_argn+1),
  itsPkg(pkg),
  itsItemArgn(item_argn)
{
DOTRACE("Tcl::VecGetterBaseCmd::VecGetterBaseCmd");
}

void Tcl::VecGetterBaseCmd::invoke() {
DOTRACE("Tcl::VecGetterBaseCmd::invoke");
  if (itsItemArgn) {
	 Tcl::ListIterator<int>
		id_itr = beginOfArg(itsItemArgn, (int*)0),
		end    =   endOfArg(itsItemArgn, (int*)0);

	 // We do the first iteration outside the loop, because if there is
	 // only one item, we want to do a regular return rather than a
	 // list-append to the return value. It is safe to assume that
	 // there is at least one item in the list, otherwise the number of
	 // arguments to the command would have been wrong.
	 void* item = itsPkg->getItemFromId(*id_itr);
	 doReturnValForItem(item);

	 while (++id_itr != end)
		{
		  void* item = itsPkg->getItemFromId(*id_itr);
		  doAppendValForItem(item);
		}
  }
  else {
	 void* item = itsPkg->getItemFromId(-1);
	 doReturnValForItem(item);
  }
}

template <class ValType>
Tcl::TVecGetterCmd<ValType>::TVecGetterCmd(TclItemPkgBase* pkg, const char* cmd_name,
														 Getter<ValType>* getter,
														 const char* usage, int item_argn) :
  TclCmd(pkg->interp(), cmd_name,
			usage ? usage : (item_argn ? "item_id(s)" : NULL), 
			item_argn+1, item_argn+1),
  VecGetterBaseCmd(pkg, cmd_name, usage, item_argn),
  itsGetter(getter)
{
DOTRACE("Tcl::TVecGetterCmd<>::TVecGetterCmd");
}

template <class ValType>
void Tcl::TVecGetterCmd<ValType>::doReturnValForItem(void* item) {
  returnVal(itsGetter->get(item));
}

template <class ValType>
void Tcl::TVecGetterCmd<ValType>::doAppendValForItem(void* item) {
  lappendVal(itsGetter->get(item));
}

// Specializations for fixed_string
template <>
void Tcl::TVecGetterCmd<const fixed_string&>::doReturnValForItem(void* item) {
  returnVal(itsGetter->get(item).c_str());
}

template <>
void Tcl::TVecGetterCmd<const fixed_string&>::doAppendValForItem(void* item) {
  lappendVal(itsGetter->get(item).c_str());
}

// Explicit instatiation requests
namespace Tcl {
template class TVecGetterCmd<int>;
template class TVecGetterCmd<bool>;
template class TVecGetterCmd<double>;
template class TVecGetterCmd<const char*>;
template class TVecGetterCmd<const fixed_string&>;
}

///////////////////////////////////////////////////////////////////////
//
// TVecSetterCmd definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::VecSetterBaseCmd::VecSetterBaseCmd(TclItemPkgBase* pkg, const char* cmd_name,
													 const char* usage, int item_argn) :
  TclCmd(pkg->interp(), cmd_name, 
			usage ? usage : (item_argn ? 
								  "item_id(s) new_value(s)" : "new_value"), 
			item_argn+2, item_argn+2),
  itsPkg(pkg),
  itsItemArgn(item_argn),
  itsValArgn(item_argn+1)
{
DOTRACE("Tcl::VecSetterBaseCmd::VecSetterBaseCmd");
}

void Tcl::VecSetterBaseCmd::invoke() {
DOTRACE("Tcl::VecSetterBaseCmd::invoke");
  vector<int> ids;
  if (itsItemArgn) {
	 getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
  }
  else {
	 ids.push_back(-1);
  }

  unsigned int num_vals;
  void* vals = getValVec(itsValArgn, ids.size(), num_vals);

  if (num_vals < 1) {
	 throw TclError("the list of new values is empty");
  }

  size_t max_valn = num_vals-1;
  DebugEvalNL(max_valn);

  for (size_t i = 0, valn = 0; i < ids.size(); ++i) {
	 DebugEval(i);
	 void* item = itsPkg->getItemFromId(ids[i]);
	 setValForItem(item, vals, valn);
	 if (valn < max_valn) ++valn;
  }

  destroyValVec(vals);
}

template <class T>
Tcl::TVecSetterCmd<T>::TVecSetterCmd(TclItemPkgBase* pkg, const char* cmd_name,
												 Setter<T>* setter,
												 const char* usage, int item_argn) :
  TclCmd(pkg->interp(), cmd_name, 
			usage ? usage : (item_argn ? 
								  "item_id(s) new_value(s)" : "new_value"), 
			item_argn+2, item_argn+2),
  VecSetterBaseCmd(pkg, cmd_name, usage, item_argn),
  itsSetter(setter)
{
DOTRACE("Tcl::TVecSetterCmd<>::TVecSetterCmd");
}

template <class T>
void* Tcl::TVecSetterCmd<T>::getValVec(int val_argn, int num_ids,
													unsigned int& num_vals) {
  vector<T>* vals = new vector<T>;
  if (num_ids == 1) {
	 T val;
	 getValFromArg(val_argn, val);
	 vals->push_back(val);
  }
  else {
	 getSequenceFromArg(val_argn, back_inserter(*vals), (T*) 0);
  }
  num_vals = vals->size();
  return static_cast<void*>(vals);
}

template <class T>
void Tcl::TVecSetterCmd<T>::setValForItem(void* item,
														void* val_vec, unsigned int valn) {
  vector<T>& vals = *(static_cast<vector<T>*>(val_vec));
  itsSetter->set(item, vals[valn]);
}

template <class T>
void Tcl::TVecSetterCmd<T>::destroyValVec(void* val_vec) {
  vector<T>* vals = static_cast<vector<T>*>(val_vec);
  delete vals;
}


// Specialization for T=const fixed_string&
template <>
void* Tcl::TVecSetterCmd<const fixed_string&>::getValVec(
  int val_argn, int num_ids, unsigned int& num_vals
) {
  vector<fixed_string>* vals = new vector<fixed_string>;
  if (num_ids == 1) {
	 vals->push_back(fixed_string());
	 vals->back() = getCstringFromArg(val_argn);
  }
  else {
	 getSequenceFromArg(val_argn, back_inserter(*vals), (const char**) 0);
  }
  num_vals = vals->size();
  return static_cast<void*>(vals);
}

template <>
void Tcl::TVecSetterCmd<const fixed_string&>::setValForItem(
  void* item, void* val_vec, unsigned int valn
) {
  vector<fixed_string>& vals = *(static_cast<vector<fixed_string>*>(val_vec));
  itsSetter->set(item, vals[valn]);
}

template <>
void Tcl::TVecSetterCmd<const fixed_string&>::destroyValVec(void* val_vec)
{
  vector<fixed_string>* vals = static_cast<vector<fixed_string>*>(val_vec);
  delete vals;
}

// Explicit instatiation requests
namespace Tcl {
template class TVecSetterCmd<int>;
template class TVecSetterCmd<bool>;
template class TVecSetterCmd<double>;
template class TVecSetterCmd<const char*>;
template class TVecSetterCmd<const fixed_string&>;
}

///////////////////////////////////////////////////////////////////////
//
// TVecAttribCmd definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
Tcl::TVecAttribCmd<T>::TVecAttribCmd(TclItemPkgBase* pkg, const char* cmd_name,
												 Attrib<T>* attrib,
												 const char* usage, int item_argn) :
  TVecGetterCmd<T>(pkg, 0, attrib, 0, item_argn),
  TVecSetterCmd<T>(pkg, 0, attrib, 0, item_argn),
  TclCmd(pkg->interp(), cmd_name,
			usage ? usage : (item_argn ?
								  "item_id(s) ?new_value(s)?" : "?new_value?"),
			item_argn+1, item_argn+2, false),
  itsObjcGet(item_argn+1),
  itsObjcSet(item_argn+2)
{
DOTRACE("Tcl::TVecAttribCmd<>::TVecAttribCmd");
}

template <class T>
void Tcl::TVecAttribCmd<T>::invoke() {
DOTRACE("Tcl::TVecAttribCmd<>::invoke");
  if      (TclCmd::objc() == itsObjcGet) { TVecGetterCmd<T>::invoke(); }
  else if (TclCmd::objc() == itsObjcSet) { TVecSetterCmd<T>::invoke(); }
  else    /* "can't happen" */           { Assert(0); }
}

// Explicit instatiation requests
namespace Tcl {
template class TVecAttribCmd<int>;
template class TVecAttribCmd<bool>;
template class TVecAttribCmd<double>;
template class TVecAttribCmd<const char*>;
template class TVecAttribCmd<const fixed_string&>;
}

///////////////////////////////////////////////////////////////////////
//
// VecActionCmd
//
///////////////////////////////////////////////////////////////////////

Tcl::VecActionCmd::VecActionCmd(TclItemPkgBase* pkg, const char* cmd_name,
										  Action* action,
										  const char* usage, int item_argn) :
  TclCmd(pkg->interp(), cmd_name, 
			usage ? usage : (item_argn ? "item_id(s)" : NULL),
			item_argn+1, item_argn+1),
  itsPkg(pkg),
  itsAction(action),
  itsItemArgn(item_argn)
{
DOTRACE("Tcl::VecActionCmd::VecActionCmd");
}

void Tcl::VecActionCmd::invoke() {
DOTRACE("Tcl::VecActionCmd::invoke");
  if (itsItemArgn) {
	 Tcl::ListIterator<int>
		id_itr = beginOfArg(itsItemArgn, (int*)0),
		end    =   endOfArg(itsItemArgn, (int*)0);

	 while (id_itr != end)
		{
		  void* item = itsPkg->getItemFromId(*id_itr);
		  itsAction->action(item);
		  ++id_itr;
		}
  }
  else {
	 void* item = itsPkg->getItemFromId(-1);
	 itsAction->action(item);
  }
}

static const char vcid_tclveccmds_cc[] = "$Header$";
#endif // !TCLVECCMDS_CC_DEFINED
