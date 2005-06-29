///////////////////////////////////////////////////////////////////////
//
// volatileobject.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Aug 21 17:17:51 2001
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

#ifndef GROOVX_NUB_VOLATILEOBJECT_CC_UTC20050626084018_DEFINED
#define GROOVX_NUB_VOLATILEOBJECT_CC_UTC20050626084018_DEFINED

#include "volatileobject.h"

#include "rutz/trace.h"

Nub::VolatileObject::VolatileObject()
{
GVX_TRACE("Nub::VolatileObject::VolatileObject");
  this->markAsVolatile();
}

Nub::VolatileObject::~VolatileObject() throw()
{
GVX_TRACE("Nub::VolatileObject::~VolatileObject");
}

void Nub::VolatileObject::destroy()
{
GVX_TRACE("Nub::VolatileObject::destroy");
  delete this;
}

static const char vcid_groovx_nub_volatileobject_cc_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_VOLATILEOBJECT_CC_UTC20050626084018_DEFINED
