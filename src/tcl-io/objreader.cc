/** @file tcl-io/objreader.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov  8 15:38:06 2005
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

#include "tcl-io/objreader.h"

#include "io/io.h"
#include "nub/ref.h"
#include "rutz/bytearray.h"
#include "rutz/error.h"
#include "rutz/trace.h"
#include "rutz/value.h"
#include "tcl/conversions.h"

tcl::obj_reader::obj_reader(const tcl::obj& x)
  :
  m_obj(x)
{
GVX_TRACE("tcl::obj_reader::obj_reader");
}

tcl::obj_reader::~obj_reader() noexcept
{
GVX_TRACE("tcl::obj_reader::~obj_reader");
}

io::version_id tcl::obj_reader::input_version_id()
{
  throw rutz::error("not implemented", SRC_POS);
}

char tcl::obj_reader::read_char(const rutz::fstring& /*name*/)
{
  return static_cast<char>(tcl::convert_to<int>(m_obj));
}

int tcl::obj_reader::read_int(const rutz::fstring& /*name*/)
{
  return tcl::convert_to<int>(m_obj);
}

bool tcl::obj_reader::read_bool(const rutz::fstring& /*name*/)
{
  return tcl::convert_to<bool>(m_obj);
}

double tcl::obj_reader::read_double(const rutz::fstring& /*name*/)
{
  return tcl::convert_to<double>(m_obj);
}

void tcl::obj_reader::read_value_obj(const rutz::fstring& /*name*/,
                                     rutz::value& v)
{
  v.set_string(tcl::convert_to<rutz::fstring>(m_obj));
}

rutz::byte_array tcl::obj_reader::read_byte_array(const rutz::fstring& name)
{
  return this->default_read_byte_array(name);
}

nub::ref<io::serializable>
tcl::obj_reader::read_object(const rutz::fstring& /*name*/)
{
  throw rutz::error("not implemented", SRC_POS);
}

nub::soft_ref<io::serializable>
tcl::obj_reader::read_weak_object(const rutz::fstring& /*name*/)
{
  throw rutz::error("not implemented", SRC_POS);
}

void tcl::obj_reader::read_owned_object(const rutz::fstring& /*name*/,
                                        nub::ref<io::serializable> /*obj*/)
{
  throw rutz::error("not implemented", SRC_POS);
}

void tcl::obj_reader::read_base_class(const rutz::fstring& /*base_class_name*/,
                                      nub::ref<io::serializable> /*base_part*/)
{
  throw rutz::error("not implemented", SRC_POS);
}

nub::ref<io::serializable>
tcl::obj_reader::read_root(io::serializable* /*root*/)
{
  throw rutz::error("not implemented", SRC_POS);
}

rutz::fstring tcl::obj_reader::read_string_impl(const rutz::fstring& /*name*/)
{
  return tcl::convert_to<rutz::fstring>(m_obj);
}
