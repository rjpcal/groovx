///////////////////////////////////////////////////////////////////////
//
// tclobjptr.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 18:30:47 2001
// written: Tue Aug  7 11:14:34 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLOBJPTR_CC_DEFINED
#define TCLOBJPTR_CC_DEFINED

#include "tcl/tclobjptr.h"

#include <tcl.h>

namespace
{
  Tcl::ObjPtr theNullObject("");
}

Tcl::ObjPtr::ObjPtr() : itsObj(theNullObject.itsObj) { incrRef(itsObj); }

void Tcl::ObjPtr::append(Tcl::ObjPtr other)
{
  ensureUnique();
  Tcl_AppendObjToObj(itsObj, other.itsObj);
}

bool Tcl::ObjPtr::isShared() const
{
  return Tcl_IsShared(itsObj);
}

void Tcl::ObjPtr::ensureUnique()
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
