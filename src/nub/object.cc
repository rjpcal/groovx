/** @file nub/object.cc base class for objects to be exposed to a
    scripting language */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Jun  5 10:26:14 2001
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

#ifndef GROOVX_NUB_OBJECT_CC_UTC20050626084019_DEFINED
#define GROOVX_NUB_OBJECT_CC_UTC20050626084019_DEFINED

#include "object.h"

#include "rutz/demangle.h"
#include "rutz/fstring.h"

#include <typeinfo>

#include "rutz/trace.h"

namespace
{
  nub::uid s_id_counter = 0;
}

nub::object::object() : m_uid(++s_id_counter)
{
GVX_TRACE("nub::object::object");
}

nub::object::~object() GVX_DTOR_NOTHROW
{
GVX_TRACE("nub::object::~object");
}

nub::uid nub::object::id() const throw()
{
  return m_uid;
}

rutz::fstring nub::object::real_typename() const
{
GVX_TRACE("nub::object::real_typename");
  return rutz::demangled_name(typeid(*this));
}

rutz::fstring nub::object::obj_typename() const
{
GVX_TRACE("nub::object::obj_typename");
  return real_typename();
}

rutz::fstring nub::object::unique_name() const
{
GVX_TRACE("nub::object::unique_name");
  return rutz::fstring(obj_typename(), "(", id(), ")");
}

static const char vcid_groovx_nub_object_cc_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_OBJECT_CC_UTC20050626084019_DEFINED
