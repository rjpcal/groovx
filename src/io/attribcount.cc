/** @file io/attribcount.cc Count the number of attributes in an io::serializable object */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Nov 11 17:16:09 2005
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

#include "io/attribcount.h"

#include "io/writer.h"

#include "nub/ref.h"

#include "rutz/trace.h"

namespace
{
  using namespace nub;

  class counting_writer : public io::writer
  {
  public:
    counting_writer() : m_count(0) {}

    virtual void write_char(const char*, char) override             { ++m_count; }
    virtual void write_int(const char*, int) override               { ++m_count; }
    virtual void write_bool(const char*, bool) override             { ++m_count; }
    virtual void write_double(const char*, double) override         { ++m_count; }
    virtual void write_cstring(const char*, const char*) override   { ++m_count; }
    virtual void write_value_obj(const char*, const rutz::value&) override { ++m_count; }

    virtual void write_byte_array(const char*,
                                  const unsigned char*,
                                  unsigned int) override
    { ++m_count; }

    virtual void write_object(const char*,
                              soft_ref<const io::serializable>) override
    { ++m_count; }

    virtual void write_owned_object(const char*,
                                    ref<const io::serializable>) override
    { ++m_count; }

    virtual void write_base_class(const char*,
                                  ref<const io::serializable>) override
    { ++m_count; }

    virtual void write_root(const io::serializable*) override {}

    unsigned int get_count() const { return m_count; }

  private:
    unsigned int m_count;
  };
}

unsigned int io::attrib_count(const io::serializable& obj)
{
GVX_TRACE("io::attrib_count");
  counting_writer counter;
  obj.write_to(counter);
  return counter.get_count();
}
