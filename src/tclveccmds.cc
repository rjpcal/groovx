///////////////////////////////////////////////////////////////////////
//
// tclveccmds.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 12:16:22 1999
// written: Thu Dec 16 15:38:17 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMDS_CC_DEFINED
#define TCLVECCMDS_CC_DEFINED

#include "tclveccmds.h"

#include "tclitempkgbase.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

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
  vector<int> ids;
  if (itsItemArgn) {
	 getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
  }
  else {
	 ids.push_back(-1);
  }
  
  void* item = itsPkg->getItemFromId(ids[0]);
  doReturnValForItem(item);
  
  for (size_t i = 1; i < ids.size(); ++i) {
	 void* item = itsPkg->getItemFromId(ids[i]);
	 doAppendValForItem(item);
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

// Oddly, aCC requires this specialization of invoke() for the type
// 'const string&', even though the code is *exactly the same* as in
// the generic templated version of invoke() above.
template <>
void Tcl::TVecGetterCmd<const string&>::doReturnValForItem(void* item) {
  returnVal(itsGetter->get(item));
}

template <>
void Tcl::TVecGetterCmd<const string&>::doAppendValForItem(void* item) {
  lappendVal(itsGetter->get(item));
}

// Explicit instatiation requests
namespace Tcl {
template class TVecGetterCmd<int>;
template class TVecGetterCmd<bool>;
template class TVecGetterCmd<double>;
template class TVecGetterCmd<const char*>;
template class TVecGetterCmd<const string&>;
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

  size_t num_vals;
  void* vals = getValVec(itsValArgn, num_vals);

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
void* Tcl::TVecSetterCmd<T>::getValVec(int val_argn, size_t& num_vals) {
  vector<T>* vals = new vector<T>;
  getSequenceFromArg(val_argn, back_inserter(*vals), (T*) 0);
  num_vals = vals->size();
  return static_cast<void*>(vals);
}

template <class T>
void Tcl::TVecSetterCmd<T>::setValForItem(void* item,
														void* val_vec, size_t valn) {
  vector<T>& vals = *(static_cast<vector<T>*>(val_vec));
  itsSetter->set(item, vals[valn]);
}

template <class T>
void Tcl::TVecSetterCmd<T>::destroyValVec(void* val_vec) {
  vector<T>* vals = static_cast<vector<T>*>(val_vec);
  delete vals;
}


// Specialization for T=const string&, since we must declare 'vals' as
// type 'vector<string>' rather than 'vector<const string&>'.
template <>
void* Tcl::TVecSetterCmd<const string&>::getValVec(int val_argn, size_t& num_vals) {
  vector<string>* vals = new vector<string>;
  getSequenceFromArg(val_argn, back_inserter(*vals), (string*) 0);
  num_vals = vals->size();
  return static_cast<void*>(vals);
}

template <>
void Tcl::TVecSetterCmd<const string&>::setValForItem(void* item,
														void* val_vec, size_t valn) {
  vector<string>& vals = *(static_cast<vector<string>*>(val_vec));
  itsSetter->set(item, vals[valn]);
}

template <>
void Tcl::TVecSetterCmd<const string&>::destroyValVec(void* val_vec) {
  vector<string>* vals = static_cast<vector<string>*>(val_vec);
  delete vals;
}

// Explicit instatiation requests
namespace Tcl {
template class TVecSetterCmd<int>;
template class TVecSetterCmd<bool>;
template class TVecSetterCmd<double>;
template class TVecSetterCmd<const char*>;
template class TVecSetterCmd<const string&>;
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
template class TVecAttribCmd<const string&>;
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
  vector<int> ids;
  if (itsItemArgn) {
	 getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
  }
  else {
	 ids.push_back(-1);
  }
  
  for (size_t i = 0; i < ids.size(); ++i) {
	 void* item = itsPkg->getItemFromId(ids[i]);
	 itsAction->action(item);
  }
}

static const char vcid_tclveccmds_cc[] = "$Header$";
#endif // !TCLVECCMDS_CC_DEFINED
