/** @file io/xmlwriter.cc io::writer implementation for writing XML
    files in the GVX format */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Jun 19 18:26:48 2003
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

#ifndef GROOVX_IO_XMLWRITER_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_XMLWRITER_CC_UTC20050626084021_DEFINED

#include "io/xmlwriter.h"

#include "io/io.h"
#include "io/writeidmap.h"
#include "io/writer.h"

#include "nub/ref.h"

#include "rutz/arrays.h"
#include "rutz/fstring.h"
#include "rutz/gzstreambuf.h"
#include "rutz/value.h"

#include <ostream>
#include <set>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::shared_ptr;

using nub::ref;
using nub::soft_ref;

class xml_writer : public io::writer
{
public:
  xml_writer(std::ostream& os);

  xml_writer(const char* filename);

  virtual ~xml_writer() noexcept;

  virtual void write_char(const char* name, char val);
  virtual void write_int(const char* name, int val);
  virtual void write_bool(const char* name, bool val);
  virtual void write_double(const char* name, double val);
  virtual void write_value_obj(const char* name, const rutz::value& v);

  virtual void write_byte_array(const char* name,
                            const unsigned char* data,
                            unsigned int length);

  virtual void write_object(const char* name,
                           nub::soft_ref<const io::serializable> obj);

  virtual void write_owned_object(const char* name,
                                nub::ref<const io::serializable> obj);

  virtual void write_base_class(const char* base_class_name,
                                nub::ref<const io::serializable> base_part);

  virtual void write_root(const io::serializable* root);

protected:
  virtual void write_cstring(const char* name, const char* val);

private:
  template <class T>
  void write_basic_type(const char* name, T val,
                        const char* type_name)
  {
    indent();
    m_buf << "<" << type_name
          << " name=\"" << name << "\""
          << " value=\"" << val << "\"/>\n";
  }

  void flatten_object(soft_ref<const io::serializable> obj, const char* name,
                      const char* xmltype);

  bool already_written(soft_ref<const io::serializable> obj) const
  {
    return ( m_written_objs.find(obj.id()) != m_written_objs.end() );
  }

  void mark_as_written(soft_ref<const io::serializable> obj)
  {
    m_written_objs.insert(obj.id());
  }

  void indent()
  {
    for (int i = 0; i < m_nest_level; ++i)
      m_buf << '\t';
  }

  void write_escaped(const char* text);

  shared_ptr<std::ostream>  m_owned_stream;
  std::ostream&             m_buf;
  std::set<nub::uid>        m_written_objs;
  int                       m_nest_level;
  io::write_id_map          m_id_map;
};

///////////////////////////////////////////////////////////////////////
//
// xml_writer member definitions
//
///////////////////////////////////////////////////////////////////////

xml_writer::xml_writer(std::ostream& os) :
  m_owned_stream(),
  m_buf(os),
  m_written_objs(),
  m_nest_level(0)
{
GVX_TRACE("xml_writer::xml_writer");
}

xml_writer::xml_writer(const char* filename) :
  m_owned_stream(rutz::ogzopen(filename)),
  m_buf(*m_owned_stream),
  m_written_objs(),
  m_nest_level(0)
{
GVX_TRACE("xml_writer::xml_writer(const char*)");
}

xml_writer::~xml_writer () noexcept
{
GVX_TRACE("xml_writer::~xml_writer");
}

void xml_writer::write_char(const char* name, char val)
{
GVX_TRACE("xml_writer::write_char");
  write_basic_type(name, val, "char");
}

void xml_writer::write_int(const char* name, int val)
{
GVX_TRACE("xml_writer::write_int");
  write_basic_type(name, val, "int");
}

void xml_writer::write_bool(const char* name, bool val)
{
GVX_TRACE("xml_writer::write_bool");
  write_basic_type(name, val, "bool");
}

void xml_writer::write_double(const char* name, double val)
{
GVX_TRACE("xml_writer::write_double");
  write_basic_type(name, val, "double");
}

void xml_writer::write_value_obj(const char* name, const rutz::value& v)
{
GVX_TRACE("xml_writer::write_value_obj");
  indent();
  m_buf << "<valobj"
        << " type=\"" << v.value_typename() << "\""
        << " name=\"" << name << "\""
        << " value=\"";
  v.print_to(m_buf);
  m_buf << "\"/>\n";
}

void xml_writer::write_byte_array(const char* name,
                             const unsigned char* data,
                             unsigned int length)
{
GVX_TRACE("xml_writer::write_byte_array");
  default_write_byte_array(name, data, length);
}

void xml_writer::write_object(const char* name,
                            soft_ref<const io::serializable> obj)
{
GVX_TRACE("xml_writer::write_object");

  if (obj.is_valid())
    {
      GVX_ASSERT(dynamic_cast<const io::serializable*>(obj.get()) != 0);

      if (already_written(obj))
        {
          indent();
          m_buf << "<objref type=\"" << obj->obj_typename() << "\""
                << " id=\"" << m_id_map.get(obj->id()) << "\""
                << " name=\"" << name << "\"/>\n";
        }
      else
        {
          flatten_object(obj, name, "object");
        }
    }
  else
    {
      indent();
      m_buf << "<objref type=\"NULL\" id=\"0\" name=\"" << name << "\"/>\n";
    }
}

void xml_writer::write_owned_object(const char* name,
                                 ref<const io::serializable> obj)
{
GVX_TRACE("xml_writer::write_owned_object");

  flatten_object(obj, name, "ownedobj");
}

void xml_writer::write_base_class(const char* base_class_name,
                                 ref<const io::serializable> base_part)
{
GVX_TRACE("xml_writer::write_base_class");

  flatten_object(base_part, base_class_name, "baseclass");
}

void xml_writer::write_root(const io::serializable* root)
{
GVX_TRACE("xml_writer::write_root");

  m_buf << "<?xml version=\"1.0\"?>\n"
        << "<!-- GroovX XML 1 -->\n";

  flatten_object(soft_ref<io::serializable>(const_cast<io::serializable*>(root)),
                "root", "object");

  m_buf.flush();
}

void xml_writer::write_cstring(const char* name, const char* val)
{
GVX_TRACE("xml_writer::write_cstring");

  indent();
  // special case for empty string:
  if (*val == '\0')
    {
      m_buf << "<string name=\"" << name << "\"/>\n";
    }
  else
    {
      m_buf << "<string name=\"" << name << "\">";
      write_escaped(val);
      m_buf << "</string>\n";
    }
}

void xml_writer::flatten_object(soft_ref<const io::serializable> obj,
                               const char* name, const char* xmltype)
{
GVX_TRACE("xml_writer::flatten_object");

  indent();
  m_buf << "<" << xmltype << " type=\"" << obj->obj_typename() << "\""
        << " id=\"" << m_id_map.get(obj->id()) << "\""
        << " name=\"" << name << "\""
        << " version=\"" << obj->class_version_id() << "\">\n";

  ++m_nest_level;
  obj->write_to(*this);
  --m_nest_level;

  mark_as_written(obj);

  indent();
  m_buf << "</" << xmltype << ">\n";
}

void xml_writer::write_escaped(const char* p)
{
GVX_TRACE("xml_writer::write_escaped");

  // Escape any special characters
  for ( ; *p != '\0'; ++p)
    {
      switch (*p)
        {
        case '<':  m_buf.write("&lt;",   4); break;
        case '>':  m_buf.write("&gt;",   4); break;
        case '&':  m_buf.write("&amp;",  5); break;
        case '"':  m_buf.write("&quot;", 6); break;
        case '\'': m_buf.write("&apos;", 6); break;
        default:   m_buf.put(*p);            break;
        }
    }
}


shared_ptr<io::writer> io::make_xml_writer(std::ostream& os)
{
  return rutz::make_shared(new xml_writer(os));
}

shared_ptr<io::writer> io::make_xml_writer(const char* filename)
{
  return rutz::make_shared(new xml_writer(filename));
}

#endif // !GROOVX_IO_XMLWRITER_CC_UTC20050626084021_DEFINED
