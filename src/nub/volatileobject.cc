/** @file nub/volatileobject.cc nub::object subclass for inherently
    unshareable objects */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Aug 21 17:17:51 2001
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

#include "volatileobject.h"

#include "rutz/trace.h"

nub::volatile_object::volatile_object()
{
GVX_TRACE("nub::volatile_object::volatile_object");
  this->mark_as_volatile();
}

nub::volatile_object::~volatile_object() noexcept
{
GVX_TRACE("nub::volatile_object::~volatile_object");
}

void nub::volatile_object::destroy()
{
GVX_TRACE("nub::volatile_object::destroy");
  delete this;
}
