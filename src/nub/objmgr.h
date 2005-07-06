/** @file nub/objmgr.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Apr 23 01:12:37 1999
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

#ifndef GROOVX_NUB_OBJMGR_H_UTC20050626084018_DEFINED
#define GROOVX_NUB_OBJMGR_H_UTC20050626084018_DEFINED

#include "nub/ref.h"

namespace rutz
{
  class fstring;
}

namespace nub
{
  namespace obj_mgr
  {
    /// Will not return 0; will throw an exception on failure.
    nub::soft_ref<nub::object> new_obj(const char* type);

    /// Will not return 0; will throw an exception on failure.
    nub::soft_ref<nub::object> new_obj(const rutz::fstring& type);

    template <class T, class S>
    nub::soft_ref<T> new_typed_obj(S type)
    {
      return dyn_cast<T>(new_obj(type));
    }
  }
}

static const char vcid_groovx_nub_objmgr_h_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_OBJMGR_H_UTC20050626084018_DEFINED
