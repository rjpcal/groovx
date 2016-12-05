/** @file nub/ref.cc smart pointers (both strong and weak) using
    intrusive ref-counting with nub::object and derivatives */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 16 09:06:27 2005
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#include "nub/ref.h"

#include "nub/objdb.h"

#include "rutz/error.h"
#include "rutz/demangle.h"

#include "rutz/debug.h"
GVX_DBG_REGISTER

bool nub::detail::is_valid_uid(nub::uid id) noexcept
{
  return nub::objectdb::instance().is_valid_uid(id);
}

nub::object* nub::detail::get_checked_item(nub::uid id)
{
  return nub::objectdb::instance().get_checked_obj(id);
}

void nub::detail::insert_item_public(nub::object* obj)
{
  nub::objectdb::instance().insert_obj(obj);
}

void nub::detail::insert_item_protected(nub::object* obj)
{
  nub::objectdb::instance().insert_obj_weak(obj);
}
