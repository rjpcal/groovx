///////////////////////////////////////////////////////////////////////
//
// tclobjptr.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 18:30:47 2001
// written: Mon Sep 17 11:25:46 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLOBJPTR_CC_DEFINED
#define TCLOBJPTR_CC_DEFINED

#include "tcl/tclobjptr.h"

#include <tcl.h>

#include "util/debug.h"

namespace
{
  Tcl::ObjPtr theNullObject("");
}

Tcl::ObjPtr::ObjPtr() : itsObj(theNullObject.itsObj) { incrRef(itsObj); }

void Tcl::ObjPtr::append(const Tcl::ObjPtr& other)
{
  ensureUnique();
  Tcl_AppendObjToObj(itsObj, other.itsObj);
}

bool Tcl::ObjPtr::isShared() const
{
  return Tcl_IsShared(itsObj);
}

void Tcl::ObjPtr::ensureUnique() const
{
  if (isShared())
    {
      Tcl_Obj* newObj = Tcl_DuplicateObj(itsObj);
      assign(newObj);
    }
}

const char* Tcl::ObjPtr::typeName() const
{
  Tcl_ObjType* type = itsObj->typePtr;

  return type ? type->name : "string";
}

void Tcl::ObjPtr::incrRef(Tcl_Obj* obj)
{
  Tcl_IncrRefCount(obj);
}

void Tcl::ObjPtr::decrRef(Tcl_Obj* obj)
{
  Tcl_DecrRefCount(obj);
}

static const char vcid_tclobjptr_cc[] = "$Header$";
#endif // !TCLOBJPTR_CC_DEFINED
