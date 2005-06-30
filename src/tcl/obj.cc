///////////////////////////////////////////////////////////////////////
//
// tclobjptr.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jul 11 18:30:47 2001
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_TCL_OBJ_CC_UTC20050628162420_DEFINED
#define GROOVX_TCL_OBJ_CC_UTC20050628162420_DEFINED

#include "tcl/obj.h"

#include <tcl.h>

Tcl::Obj::Obj() : itsObj(Tcl_NewObj()) { incrRef(itsObj); }

void Tcl::Obj::append(const Tcl::Obj& other)
{
  ensureUnique();
  Tcl_AppendObjToObj(itsObj, other.itsObj);
}

bool Tcl::Obj::is_shared() const
{
  return Tcl_IsShared(itsObj);
}

void Tcl::Obj::ensureUnique() const
{
  if (is_shared())
    {
      Tcl_Obj* new_obj = Tcl_DuplicateObj(itsObj);
      assign(new_obj);
    }
}

const char* Tcl::Obj::typeName() const
{
  Tcl_ObjType* type = itsObj->typePtr;

  return type ? type->name : "(none)";
}

void Tcl::Obj::incrRef(Tcl_Obj* obj)
{
  Tcl_IncrRefCount(obj);
}

void Tcl::Obj::decrRef(Tcl_Obj* obj)
{
  Tcl_DecrRefCount(obj);
}

static const char vcid_groovx_tcl_obj_cc_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_OBJ_CC_UTC20050628162420_DEFINED
