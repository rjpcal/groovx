/** @file nub/object.cc base class for objects to be exposed to a
    scripting language */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Jun  5 10:26:14 2001
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

#include "object.h"

#include "rutz/demangle.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

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

nub::object::~object() noexcept
{
GVX_TRACE("nub::object::~object");
}

nub::uid nub::object::id() const noexcept
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
  return rutz::sfmt("%s(%lu)", obj_typename().c_str(), id());
}
