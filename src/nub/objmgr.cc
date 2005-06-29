///////////////////////////////////////////////////////////////////////
//
// objmgr.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Apr 23 01:13:16 1999
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

#ifndef GROOVX_NUB_OBJMGR_CC_UTC20050626084019_DEFINED
#define GROOVX_NUB_OBJMGR_CC_UTC20050626084019_DEFINED

#include "objmgr.h"

#include "nub/objfactory.h"

#include "rutz/fstring.h"

#include "rutz/trace.h"

Nub::SoftRef<Nub::Object> Nub::ObjMgr::newObj(const char* type)
{
  return newObj(rutz::fstring(type));
}

Nub::SoftRef<Nub::Object> Nub::ObjMgr::newObj(const rutz::fstring& type)
{
GVX_TRACE("Nub::ObjMgr::newObj(const fstring&)");
  return SoftRef<Object>(ObjFactory::theOne().new_checked_object(type));
}

static const char vcid_groovx_nub_objmgr_cc_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_OBJMGR_CC_UTC20050626084019_DEFINED
