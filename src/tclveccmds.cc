///////////////////////////////////////////////////////////////////////
//
// tclveccmds.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 12:16:22 1999
// written: Thu Jul 12 14:06:25 2001
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

Tcl::VecGetterBaseCmd::VecGetterBaseCmd(TclItemPkgBase* pkg,
                                        const char* cmd_name,
                                        const char* usage, int item_argn) :
  VecCmd(pkg->interp(), cmd_name,
         usage ? usage : (item_argn ? "item_id(s)" : (char *) 0),
         item_argn,
         item_argn+1, item_argn+1),
  itsPkg(pkg),
  itsItemArgn(item_argn)
{
DOTRACE("Tcl::VecGetterBaseCmd::VecGetterBaseCmd");
}

Tcl::VecGetterBaseCmd::~VecGetterBaseCmd() {}

void Tcl::VecGetterBaseCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::VecGetterBaseCmd::invoke");
  void* item = itsPkg->getItemFromContext(ctx);
  doReturnValForItem(item, ctx);
}

template <class ValType>
Tcl::TVecGetterCmd<ValType>::TVecGetterCmd(TclItemPkgBase* pkg,
                                           const char* cmd_name,
                                           shared_ptr<Getter<ValType> > getter,
                                           const char* usage, int item_argn) :
  VecCmd(pkg->interp(), cmd_name,
         usage ? usage : (item_argn ? "item_id(s)" : (char *) 0),
         item_argn,
         item_argn+1, item_argn+1),
  VecGetterBaseCmd(pkg, cmd_name, usage, item_argn),
  itsGetter(getter)
{
DOTRACE("Tcl::TVecGetterCmd<>::TVecGetterCmd");
}

template <class ValType>
Tcl::TVecGetterCmd<ValType>::~TVecGetterCmd() {}

template <class ValType>
void Tcl::TVecGetterCmd<ValType>::doReturnValForItem(void* item,
                                                     Context& ctx)
{
  ctx.setResult(itsGetter->get(item));
}

template <class ValType>
void Tcl::TVecGetterCmd<ValType>::doAppendValForItem(void* item,
                                                     Tcl::List& listObj)
{
  listObj.append(itsGetter->get(item));
}

// Specializations for fixed_string
template <>
void Tcl::TVecGetterCmd<fixed_string>::doReturnValForItem(void* item,
                                                          Context& ctx)
{
  ctx.setResult(itsGetter->get(item).c_str());
}

template <>
void Tcl::TVecGetterCmd<fixed_string>::doAppendValForItem(void* item,
                                                          Tcl::List& listObj)
{
  listObj.append(itsGetter->get(item).c_str());
}

template <>
void Tcl::TVecGetterCmd<const fixed_string&>::doReturnValForItem(
  void* item,
  Context& ctx
) {
  ctx.setResult(itsGetter->get(item).c_str());
}

template <>
void Tcl::TVecGetterCmd<const fixed_string&>::doAppendValForItem(
  void* item,
  Tcl::List& listObj
) {
  listObj.append(itsGetter->get(item).c_str());
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

Tcl::VecSetterBaseCmd::VecSetterBaseCmd(TclItemPkgBase* pkg,
                                        const char* cmd_name,
                                        const char* usage, int item_argn) :
  VecCmd(pkg->interp(), cmd_name,
         usage ? usage : (item_argn ?
                          "item_id(s) new_value(s)" : "new_value"),
         item_argn,
         item_argn+2, item_argn+2),
  itsPkg(pkg),
  itsItemArgn(item_argn),
  itsValArgn(item_argn+1)
{
DOTRACE("Tcl::VecSetterBaseCmd::VecSetterBaseCmd");
}

Tcl::VecSetterBaseCmd::~VecSetterBaseCmd() {}

void Tcl::VecSetterBaseCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::VecSetterBaseCmd::invoke");
  if (itsItemArgn) {
    invokeForItemArgn(ctx, itsItemArgn, itsValArgn);
  }
  else {
    void* item = itsPkg->getItemFromContext(ctx);
    setSingleItem(ctx, item, itsValArgn);
  }
}

template <class T>
Tcl::TVecSetterCmd<T>::TVecSetterCmd(TclItemPkgBase* pkg,
                                     const char* cmd_name,
                                     shared_ptr<Setter<T> > setter,
                                     const char* usage, int item_argn) :
  Base(pkg, cmd_name, setter, usage, item_argn),
  VecCmd(pkg->interp(), cmd_name,
         usage ? usage : (item_argn ?
                          "item_id(s) new_value(s)" : "new_value"),
         item_argn,
         item_argn+2, item_argn+2)
{}

template <class T>
Tcl::TVecSetterCmd<T>::~TVecSetterCmd() {}

template <class Traits>
Tcl::TrVecSetterCmd<Traits>::TrVecSetterCmd(
  TclItemPkgBase* pkg, const char* cmd_name,
  shared_ptr<Setter<value_type> > setter,
  const char* usage, int item_argn
) :
  VecCmd(pkg->interp(), cmd_name,
         usage ? usage : (item_argn ?
                          "item_id(s) new_value(s)" : "new_value"),
         item_argn,
         item_argn+2, item_argn+2),
  VecSetterBaseCmd(pkg, cmd_name, usage, item_argn),
  itsSetter(setter)
{
DOTRACE("Tcl::TrVecSetterCmd<>::TrVecSetterCmd");
}

template <class Traits>
Tcl::TrVecSetterCmd<Traits>::~TrVecSetterCmd() {}

template <class Traits>
void Tcl::TrVecSetterCmd<Traits>::setSingleItem(Tcl::Context& ctx,
                                                void* item, int val_argn)
{
DOTRACE("Tcl::TrVecSetterCmd<Traits>::setSingleItem");
  stack_type val = ctx.getValFromArg(val_argn, (stack_type*)0);
  itsSetter->set(item, val);
}

template <class Traits>
void Tcl::TrVecSetterCmd<Traits>::invokeForItemArgn(Tcl::Context& ctx,
                                                    int item_argn, int val_argn)
{
DOTRACE("Tcl::TrVecSetterCmd<Traits>::invokeForItemArgn");

  void* item = pkg()->getItemFromContext(ctx);
  stack_type val = ctx.getValFromArg(val_argn, (stack_type*)0);
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
                                     const char* usage, int item_argn) :
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
                                const char* usage, int item_argn) :
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
