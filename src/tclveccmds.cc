///////////////////////////////////////////////////////////////////////
//
// tclveccmds.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 12:16:22 1999
// written: Thu Jul 12 14:35:19 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMDS_CC_DEFINED
#define TCLVECCMDS_CC_DEFINED

#include "tcl/tclveccmds.h"

#include "tcl/tclerror.h"
#include "tcl/tclitempkgbase.h"

#include "util/strings.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TVecGetterCmd definitions
//
///////////////////////////////////////////////////////////////////////

template <class ValType>
Tcl::TVecGetterCmd<ValType>::TVecGetterCmd(TclItemPkgBase* pkg,
                                           const char* cmd_name,
                                           shared_ptr<Getter<ValType> > getter,
                                           const char* usage,
                                           unsigned int item_argn) :
  VecCmd(pkg->interp(), cmd_name,
         usage ? usage : (item_argn ? "item_id(s)" : (char *) 0),
         item_argn,
         item_argn+1, item_argn+1),
  itsPkg(pkg),
  itsItemArgn(item_argn),
  itsGetter(getter)
{
DOTRACE("Tcl::TVecGetterCmd<>::TVecGetterCmd");
}

template <class ValType>
Tcl::TVecGetterCmd<ValType>::~TVecGetterCmd()
{}

template <class ValType>
void Tcl::TVecGetterCmd<ValType>::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::TVecGetterCmd<>::invoke");
  void* item = itsPkg->getItemFromContext(ctx);
  ctx.setResult(itsGetter->get(item));
}


// Explicit instatiation requests
namespace Tcl
{
  template class TVecGetterCmd<int>;
  template class TVecGetterCmd<unsigned int>;
  template class TVecGetterCmd<unsigned long>;
  template class TVecGetterCmd<bool>;
  template class TVecGetterCmd<double>;
  template class TVecGetterCmd<const char*>;
  template class TVecGetterCmd<fixed_string>;
  template class TVecGetterCmd<const fixed_string&>;
}

///////////////////////////////////////////////////////////////////////
//
// TVecSetterCmd definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
Tcl::TVecSetterCmd<T>::TVecSetterCmd(
  TclItemPkgBase* pkg, const char* cmd_name,
  shared_ptr<Setter<value_type> > setter,
  const char* usage, unsigned int item_argn
) :
  VecCmd(pkg->interp(), cmd_name,
         usage ? usage : (item_argn ?
                          "item_id(s) new_value(s)" : "new_value"),
         item_argn,
         item_argn+2, item_argn+2),
  itsPkg(pkg),
  itsItemArgn(item_argn),
  itsValArgn(item_argn+1),
  itsSetter(setter)
{
DOTRACE("Tcl::TVecSetterCmd<>::TVecSetterCmd");
}

template <class T>
Tcl::TVecSetterCmd<T>::~TVecSetterCmd() {}

template <class T>
void Tcl::TVecSetterCmd<T>::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::TVecSetterCmd<>::invoke");
  void* item = itsPkg->getItemFromContext(ctx);
  stack_type val = ctx.getValFromArg(itsValArgn, (stack_type*)0);
  itsSetter->set(item, val);
}

// Explicit instatiation requests
namespace Tcl
{
  template class TVecSetterCmd<int>;
  template class TVecSetterCmd<unsigned int>;
  template class TVecSetterCmd<unsigned long>;
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
                                     shared_ptr<Getter<T> > getter,
                                     shared_ptr<Setter<T> > setter,
                                     const char* usage,
                                     unsigned int item_argn) :
  TVecGetterCmd<T>(pkg, 0, getter, 0, item_argn),
  TVecSetterCmd<T>(pkg, 0, setter, 0, item_argn),
  VecCmd(pkg->interp(), cmd_name,
         usage ? usage : (item_argn ?
                          "item_id(s) ?new_value(s)?" : "?new_value?"),
         item_argn,
         item_argn+1, item_argn+2, false),
  itsObjcGet(item_argn+1),
  itsObjcSet(item_argn+2)
{
DOTRACE("Tcl::TVecAttribCmd<>::TVecAttribCmd");
}

template <class T>
Tcl::TVecAttribCmd<T>::~TVecAttribCmd() {}

template <class T>
void Tcl::TVecAttribCmd<T>::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::TVecAttribCmd<>::invoke");
  if      (ctx.objc() == itsObjcGet) { TVecGetterCmd<T>::invoke(ctx); }
  else if (ctx.objc() == itsObjcSet) { TVecSetterCmd<T>::invoke(ctx); }
  else    /* "can't happen" */       { Assert(0); }
}

// Explicit instatiation requests
namespace Tcl
{
  template class TVecAttribCmd<int>;
  template class TVecAttribCmd<unsigned int>;
  template class TVecAttribCmd<unsigned long>;
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
                                shared_ptr<Action> action,
                                const char* usage,
                                unsigned int item_argn) :
  VecCmd(pkg->interp(), cmd_name,
         usage ? usage : (item_argn ? "item_id(s)" : (char *) 0),
         item_argn,
         item_argn+1, item_argn+1),
  itsPkg(pkg),
  itsAction(action),
  itsItemArgn(item_argn)
{
DOTRACE("Tcl::VecActionCmd::VecActionCmd");
}

Tcl::VecActionCmd::~VecActionCmd() {}

void Tcl::VecActionCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::VecActionCmd::invoke");
  void* item = itsPkg->getItemFromContext(ctx);
  itsAction->action(item);
}

static const char vcid_tclveccmds_cc[] = "$Header$";
#endif // !TCLVECCMDS_CC_DEFINED
