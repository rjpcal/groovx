/** @file nub/objmgr.cc create objects given a type name */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Apr 23 01:13:16 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "objmgr.h"

#include "nub/objfactory.h"

#include "rutz/fstring.h"

#include "rutz/trace.h"

nub::soft_ref<nub::object> nub::obj_mgr::new_obj(const char* type)
{
  return new_obj(rutz::fstring(type));
}

nub::soft_ref<nub::object> nub::obj_mgr::new_obj(const rutz::fstring& type)
{
GVX_TRACE("nub::obj_mgr::new_obj(const fstring&)");
  return soft_ref<object>(obj_factory::instance().new_checked_object(type));
}
