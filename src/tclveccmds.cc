///////////////////////////////////////////////////////////////////////
//
// tclveccmds.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 12:16:22 1999
// written: Fri Jul 13 12:17:09 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMDS_CC_DEFINED
#define TCLVECCMDS_CC_DEFINED

#include "tcl/tclveccmds.h"

#include "tcl/tclerror.h"
#include "tcl/tclitempkgbase.h"
#include "tcl/tclvalue.h"

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
Tcl::TVecGetterCmd<ValType>::TVecGetterCmd(Tcl_Interp* interp,
                                           ItemFetcher* fetcher,
                                           const char* cmd_name,
                                           shared_ptr<Getter<ValType> > getter,
                                           const char* usage,
                                           unsigned int item_argn) :
  VecCmd(interp, cmd_name,
         usage ? usage : (item_argn ? "item_id(s)" : (char *) 0),
         item_argn,
         item_argn+1, item_argn+1),
  itsFetcher(fetcher),
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
  void* item = itsFetcher->getItemFromContext(ctx);
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
  template class TVecGetterCmd<TclValue>;
}

///////////////////////////////////////////////////////////////////////
//
// TVecSetterCmd definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
Tcl::TVecSetterCmd<T>::TVecSetterCmd(
  Tcl_Interp* interp,
  ItemFetcher* fetcher, const char* cmd_name,
  shared_ptr<Setter<value_type> > setter,
  const char* usage, unsigned int item_argn
) :
  VecCmd(interp, cmd_name,
         usage ? usage : (item_argn ?
                          "item_id(s) new_value(s)" : "new_value"),
         item_argn,
         item_argn+2, item_argn+2),
  itsFetcher(fetcher),
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
  void* item = itsFetcher->getItemFromContext(ctx);
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
  template class TVecSetterCmd<fixed_string>;
  template class TVecSetterCmd<const fixed_string&>;
  template class TVecSetterCmd<TclValue>;
}

///////////////////////////////////////////////////////////////////////
//
// VecActionCmd
//
///////////////////////////////////////////////////////////////////////

Tcl::VecActionCmd::VecActionCmd(Tcl_Interp* interp,
                                ItemFetcher* fetcher, const char* cmd_name,
                                shared_ptr<Action> action,
                                const char* usage,
                                unsigned int item_argn) :
  VecCmd(interp, cmd_name,
         usage ? usage : (item_argn ? "item_id(s)" : (char *) 0),
         item_argn,
         item_argn+1, item_argn+1),
  itsFetcher(fetcher),
  itsAction(action),
  itsItemArgn(item_argn)
{
DOTRACE("Tcl::VecActionCmd::VecActionCmd");
}

Tcl::VecActionCmd::~VecActionCmd() {}

void Tcl::VecActionCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::VecActionCmd::invoke");
  void* item = itsFetcher->getItemFromContext(ctx);
  itsAction->action(item);
}

static const char vcid_tclveccmds_cc[] = "$Header$";
#endif // !TCLVECCMDS_CC_DEFINED
