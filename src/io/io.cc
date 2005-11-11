/** @file io/io.cc abstract base class for serializable objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Mar  9 20:25:02 1999
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

#ifndef GROOVX_IO_IO_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_IO_CC_UTC20050626084021_DEFINED

#include "io/io.h"

#include "io/writer.h"

#include "nub/ref.h"

#include "rutz/fstring.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace
{
  using namespace nub;

  class counting_writer : public io::writer
  {
  public:
    counting_writer() : m_count(0) {}

    virtual void write_char(const char*, char)             { ++m_count; }
    virtual void write_int(const char*, int)               { ++m_count; }
    virtual void write_bool(const char*, bool)             { ++m_count; }
    virtual void write_double(const char*, double)         { ++m_count; }
    virtual void write_cstring(const char*, const char*)   { ++m_count; }
    virtual void write_value_obj(const char*, const rutz::value&) { ++m_count; }

    virtual void write_byte_array(const char*,
                                  const unsigned char*,
                                  unsigned int)
    { ++m_count; }

    virtual void write_object(const char*,
                              soft_ref<const io::serializable>)
    { ++m_count; }

    virtual void write_owned_object(const char*,
                                    ref<const io::serializable>)
    { ++m_count; }

    virtual void write_base_class(const char*,
                                  ref<const io::serializable>)
    { ++m_count; }

    virtual void write_root(const io::serializable*) {}

    unsigned int get_count() const { return m_count; }

  private:
    unsigned int m_count;
  };
}

///////////////////////////////////////////////////////////////////////
//
// io member definitions
//
///////////////////////////////////////////////////////////////////////

io::serializable::serializable() throw()
{
GVX_TRACE("io::serializable::serializable");
  dbg_eval_nl(3, this);
}

// Must be defined out of line to avoid duplication of io's vtable
io::serializable::~serializable() GVX_DTOR_NOTHROW
{
GVX_TRACE("io::serializable::~serializable");
}

unsigned int io::serializable::attrib_count() const
{
GVX_TRACE("io::serializable::attrib_count");
  counting_writer counter;
  this->write_to(counter);
  return counter.get_count();
}

io::version_id io::serializable::class_version_id() const
{
GVX_TRACE("io::serializable::class_version_id");
  return 0;
}

static const char vcid_groovx_io_io_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_IO_CC_UTC20050626084021_DEFINED
