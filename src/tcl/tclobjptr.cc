///////////////////////////////////////////////////////////////////////
//
// tclobjptr.cc
//
// Copyright (c) 2001-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jul 11 18:30:47 2001
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLOBJPTR_CC_DEFINED
#define TCLOBJPTR_CC_DEFINED

#include "tcl/tclobjptr.h"

#include <tcl.h>

Tcl::ObjPtr::ObjPtr() : itsObj(Tcl_NewObj()) { incrRef(itsObj); }

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
