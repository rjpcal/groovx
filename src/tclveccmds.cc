///////////////////////////////////////////////////////////////////////
//
// tclveccmds.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 12:16:22 1999
// written: Wed Dec  8 01:36:23 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMDS_CC_DEFINED
#define TCLVECCMDS_CC_DEFINED

#include "tclveccmds.h"

#include "tclitempkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// TVecGetterCmd definitions
//
///////////////////////////////////////////////////////////////////////

template <class ValType>
Tcl::TVecGetterCmd<ValType>::TVecGetterCmd(TclItemPkg* pkg, const char* cmd_name,
														 Getter<ValType>* getter,
														 const char* usage, int item_argn) :
  TclCmd(pkg->interp(), cmd_name, 
			usage ? usage : (item_argn ? "item_id(s)" : NULL), 
			item_argn+1, item_argn+1),
  itsPkg(pkg),
  itsGetter(getter), 
  itsItemArgn(item_argn)
{
DOTRACE("Tcl::TVecGetterCmd<>::TVecGetterCmd");
}

template <class ValType>
void Tcl::TVecGetterCmd<ValType>::invoke() {
DOTRACE("Tcl::TVecGetterCmd<>::invoke");
  vector<int> ids;
  if (itsItemArgn) {
	 getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
  }
  else {
	 ids.push_back(-1);
  }
  
  void* item = itsPkg->getItemFromId(ids[0]);
  returnVal(itsGetter->get(item));
  
  for (size_t i = 1; i < ids.size(); ++i) {
	 void* item = itsPkg->getItemFromId(ids[i]);
	 lappendVal(itsGetter->get(item));
  }
}

// Oddly, aCC requires this specialization of invoke() for the type
// 'const string&', even though the code is *exactly the same* as in
// the generic templated version of invoke() above.
template<>
void Tcl::TVecGetterCmd<const string&>::invoke() {
  vector<int> ids;
  if (itsItemArgn) {
	 getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
  }
  else {
	 ids.push_back(-1);
  }
  
  void* item = itsPkg->getItemFromId(ids[0]);
  returnVal(itsGetter->get(item));
  
  for (size_t i = 1; i < ids.size(); ++i) {
	 void* item = itsPkg->getItemFromId(ids[i]);
	 lappendVal(itsGetter->get(item));
  }
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

template <class T>
Tcl::TVecSetterCmd<T>::TVecSetterCmd(TclItemPkg* pkg, const char* cmd_name,
												 Setter<T>* setter,
												 const char* usage, int item_argn) :
  TclCmd(pkg->interp(), cmd_name, 
			usage ? usage : (item_argn ? 
								  "item_id(s) new_value(s)" : "new_value"), 
			item_argn+2, item_argn+2),
  itsPkg(pkg),
  itsSetter(setter),
  itsItemArgn(item_argn),
  itsValArgn(item_argn+1)
{
DOTRACE("Tcl::TVecSetterCmd<>::TVecSetterCmd");
}

template <class T>
void Tcl::TVecSetterCmd<T>::invoke() {
DOTRACE("Tcl::TVecSetterCmd<>::invoke");
  vector<int> ids;
  if (itsItemArgn) {
	 getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
  }
  else {
	 ids.push_back(-1);
  }

  vector<T> vals;
  if (ids.size() == 1) {
	 T val;
	 getValFromArg(itsValArgn, val);
	 vals.push_back(val);
  }
  else {
	 getSequenceFromArg(itsValArgn, back_inserter(vals), (T*) 0);
  }

  if (vals.size() < 1) {
	 throw TclError("the list of new values is empty");
  }

  size_t max_valn = vals.size()-1;
  DebugEvalNL(max_valn);

  for (size_t i = 0, valn = 0; i < ids.size(); ++i) {
	 DebugEval(i);
	 void* item = itsPkg->getItemFromId(ids[i]);
	 itsSetter->set(item, vals[valn]);
	 if (valn < max_valn) ++valn;
  }

  returnVoid();
}


// Specialization for T=const string&, since we must declare 'vals' as
// type 'vector<string>' rather than 'vector<const string&>'.
template<>
void Tcl::TVecSetterCmd<const string&>::invoke() {
  vector<int> ids;
  if (itsItemArgn) {
	 getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
  }
  else {
	 ids.push_back(-1);
  }

  vector<string> vals;
  if (ids.size() == 1) {
	 string val;
	 getValFromArg(itsValArgn, val);
	 vals.push_back(val);
  }
  else {
	 getSequenceFromArg(itsValArgn, back_inserter(vals), (string*) 0);
  }

  if (vals.size() < 1) {
	 throw TclError("the list of new values is empty");
  }

  size_t max_valn = vals.size()-1;

  for (size_t i = 0, valn = 0; i < ids.size(); ++i) {
	 void* item = itsPkg->getItemFromId(ids[i]);
	 itsSetter->set(item, vals[valn]);
	 if (valn < max_valn) ++valn;
  }

  returnVoid();
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
Tcl::TVecAttribCmd<T>::TVecAttribCmd(TclItemPkg* pkg, const char* cmd_name,
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
Tcl::TVecAttribCmd<T>::~TVecAttribCmd()
{
DOTRACE("Tcl::TVecAttribCmd<>::~TVecAttribCmd");
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

Tcl::VecActionCmd::VecActionCmd(TclItemPkg* pkg, const char* cmd_name,
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
