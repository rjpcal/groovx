/** @file tcl-io/objwriter.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov  8 15:38:56 2005
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

#ifndef GROOVX_TCL_IO_OBJWRITER_CC_UTC20051108233856_DEFINED
#define GROOVX_TCL_IO_OBJWRITER_CC_UTC20051108233856_DEFINED

#include "tcl-io/objwriter.h"

#include "io/io.h"
#include "nub/ref.h"
#include "rutz/error.h"
#include "rutz/trace.h"
#include "tcl/conversions.h"

tcl::obj_writer::obj_writer()
{
GVX_TRACE("tcl::obj_writer::obj_writer");
}

tcl::obj_writer::~obj_writer() noexcept
{
GVX_TRACE("tcl::obj_writer::~obj_writer");
}

void tcl::obj_writer::write_char(const char* /*name*/, char val)
{
  const char arr[2] = { val, '\0' };
  m_obj = tcl::convert_from(&arr[0]);
}

void tcl::obj_writer::write_int(const char* /*name*/, int val)
{
  m_obj = tcl::convert_from(val);
}

void tcl::obj_writer::write_bool(const char* /*name*/, bool val)
{
  m_obj = tcl::convert_from(val);
}

void tcl::obj_writer::write_double(const char* /*name*/, double val)
{
  m_obj = tcl::convert_from(val);
}

void tcl::obj_writer::write_value_obj(const char* /*name*/, const rutz::value& v)
{
  m_obj = tcl::convert_from(v);
}

void tcl::obj_writer::write_byte_array(const char* name,
                                 const unsigned char* data,
                                 unsigned int length)
{
  this->default_write_byte_array(name, data, length);
}

void tcl::obj_writer::write_object(const char* /*name*/,
                             nub::soft_ref<const io::serializable> /*obj*/)
{
  throw rutz::error("not supported", SRC_POS);
}

void tcl::obj_writer::write_owned_object(const char* /*name*/,
                                   nub::ref<const io::serializable> /*obj*/)
{
  throw rutz::error("not supported", SRC_POS);
}

void tcl::obj_writer::write_base_class(const char* /*base_class_name*/,
                                 nub::ref<const io::serializable> /*base_part*/)
{
  throw rutz::error("not supported", SRC_POS);
}

void tcl::obj_writer::write_root(const io::serializable* /*root*/)
{
  throw rutz::error("not supported", SRC_POS);
}

void tcl::obj_writer::write_cstring(const char* /*name*/, const char* val)
{
  m_obj = tcl::convert_from(val);
}

#endif // !GROOVX_TCL_IO_OBJWRITER_CC_UTC20051108233856DEFINED
