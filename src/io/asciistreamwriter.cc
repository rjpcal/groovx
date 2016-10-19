/** @file io/asciistreamwriter.cc write io::serializable objects in the ASW
    format */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun  7 13:05:57 1999
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

#ifndef GROOVX_IO_ASCIISTREAMWRITER_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_ASCIISTREAMWRITER_CC_UTC20050626084021_DEFINED

#include "io/asciistreamwriter.h"

#include "io/attribcount.h"
#include "io/io.h"
#include "io/writeidmap.h"
#include "io/writer.h"

#include "nub/ref.h"

#include "rutz/arrays.h"
#include "rutz/fstring.h"
#include "rutz/gzstreambuf.h"
#include "rutz/value.h"

#include <ostream>
#include <string>
#include <set>
#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;
using rutz::shared_ptr;

using nub::ref;
using nub::soft_ref;

using std::vector;
using std::set;

namespace
{
  const char* ATTRIB_ENDER = "^\n";

  void add_escapes(std::string& text)
  {
  GVX_TRACE("<asciistreamwriter.cc>::add_escapes");
    // Escape any special characters
    for (size_t pos = 0; pos < text.length(); /* ++ done in loop body */ )
      {
        switch (text[pos])
          {
          case '\\': // i.e., a single backslash
            text.replace(pos, 1, "\\\\"); // i.e., two backslashes
            pos += 2;
            break;
          case '^':
            text.replace(pos, 1, "\\c");
            pos += 2;
            break;
          case '{':
            text.replace(pos, 1, "\\{");
            pos += 2;
            break;
          case '}':
            text.replace(pos, 1, "\\}");
            pos += 2;
            break;
          default:
            ++pos;
            break;
          }
      }
  }

  class asw_writer : public io::writer
  {
  public:
    asw_writer(std::ostream& os);

    asw_writer(const char* filename);

    virtual ~asw_writer() noexcept;

    virtual void write_char(const char* name, char val) override;
    virtual void write_int(const char* name, int val) override;
    virtual void write_bool(const char* name, bool val) override;
    virtual void write_double(const char* name, double val) override;
    virtual void write_value_obj(const char* name, const rutz::value& v) override;

    virtual void write_byte_array(const char* name,
                                  const unsigned char* data,
                                  unsigned int length) override
    { default_write_byte_array(name, data, length); }

    virtual void write_object(const char* name,
                              nub::soft_ref<const io::serializable> obj) override;

    virtual void write_owned_object(const char* name,
                                    nub::ref<const io::serializable> obj) override;

    virtual void write_base_class(const char* base_class_name,
                                  nub::ref<const io::serializable> base_part) override;

    virtual void write_root(const io::serializable* root) override;

  protected:
    virtual void write_cstring(const char* name, const char* val) override;

  private:
    shared_ptr<std::ostream>                           m_owned_stream;
    std::ostream&                                      m_buf;
    mutable vector<soft_ref<const io::serializable> >  m_pending_objs;
    set<soft_ref<const io::serializable> >             m_written_objs;
    io::write_id_map                                   m_id_map;

    void add_pending_object(soft_ref<const io::serializable> obj)
    {
      if ( !already_written(obj) )
        {
          m_pending_objs.push_back(obj);
        }
    }

    bool already_written(soft_ref<const io::serializable> obj) const
    {
      return ( m_written_objs.find(obj) !=
               m_written_objs.end() );
    }

    void mark_as_written(soft_ref<const io::serializable> obj)
    {
      m_written_objs.insert(obj);
    }

    void flatten_object(soft_ref<const io::serializable> obj);

    template <class T>
    void write_basic_type(const char* name, T val,
                          const char* string_typename)
    {
      m_buf << string_typename << " "
            << name << " := "
            << val << ATTRIB_ENDER;
    }

    void write_string_type(const char* name, const char* val)
    {
      std::string escaped_val(val);
      const size_t val_length = escaped_val.length();
      add_escapes(escaped_val);

      m_buf << "cstring "
            << name << " := "
            << val_length << " " << escaped_val.c_str() << ATTRIB_ENDER;
    }
  };

  ///////////////////////////////////////////////////////////////////////
  //
  // asw_writer member definitions
  //
  ///////////////////////////////////////////////////////////////////////

  asw_writer::asw_writer(std::ostream& os) :
    m_owned_stream(),
    m_buf(os),
    m_pending_objs(),
    m_written_objs()
  {
  GVX_TRACE("asw_writer::asw_writer");
  }

  asw_writer::asw_writer(const char* filename) :
    m_owned_stream(rutz::ogzopen(filename)),
    m_buf(*m_owned_stream),
    m_pending_objs(),
    m_written_objs()
  {
  GVX_TRACE("asw_writer::asw_writer(const char*)");
  }

  asw_writer::~asw_writer () noexcept
  {
  GVX_TRACE("asw_writer::~asw_writer");
  }

  void asw_writer::write_char(const char* name, char val)
  {
  GVX_TRACE("asw_writer::write_char");
    write_basic_type(name, val, "char");
  }

  void asw_writer::write_int(const char* name, int val)
  {
  GVX_TRACE("asw_writer::write_int");
    write_basic_type(name, val, "int");
  }

  void asw_writer::write_bool(const char* name, bool val)
  {
  GVX_TRACE("asw_writer::write_bool");
    write_basic_type(name, val, "bool");
  }

  void asw_writer::write_double(const char* name, double val)
  {
  GVX_TRACE("asw_writer::write_double");
    write_basic_type(name, val, "double");
  }

  void asw_writer::write_cstring(const char* name, const char* val)
  {
  GVX_TRACE("asw_writer::write_cstring");
    write_string_type(name, val);
  }

  void asw_writer::write_value_obj(const char* name,
                                   const rutz::value& v)
  {
  GVX_TRACE("asw_writer::write_value_obj");

    write_basic_type<const rutz::value&>(name, v, v.value_typename().c_str());
  }

  void asw_writer::write_object(const char* name,
                                soft_ref<const io::serializable> obj)
  {
  GVX_TRACE("asw_writer::write_object");

    fstring type = "NULL";
    nub::uid id = 0;

    if (obj.is_valid())
      {
        GVX_ASSERT(dynamic_cast<const io::serializable*>(obj.get()) != 0);

        type = obj->obj_typename();
        id = m_id_map.get(obj->id());

        add_pending_object(obj);
      }

    m_buf << type << " "
          << name << " := "
          << id << ATTRIB_ENDER;
  }

  void asw_writer::write_owned_object(const char* name,
                                      ref<const io::serializable> obj)
  {
  GVX_TRACE("asw_writer::write_owned_object");

    fstring type = obj->obj_typename().c_str();

    m_buf << type.c_str() << ' ' << name << " := ";

    flatten_object(obj);

    m_buf << ATTRIB_ENDER;
  }

  void asw_writer::write_base_class(const char* base_class_name,
                                    ref<const io::serializable> base_part)
  {
  GVX_TRACE("asw_writer::write_base_class");
    write_owned_object(base_class_name, base_part);
  }

  void asw_writer::write_root(const io::serializable* root)
  {
  GVX_TRACE("asw_writer::write_root");
    m_pending_objs.clear();
    m_written_objs.clear();

    // need the const_cast here because:
    // (1) soft_ref constructor will optionally call Detail::insert_item()
    // (2) insert_item() will put the object in the nub::objectdb
    // (3) objects in the nub::objectdb are non-const since they can be
    //     retrieved and modified
    m_pending_objs.push_back
      (soft_ref<io::serializable>(const_cast<io::serializable*>(root)));

    while ( !m_pending_objs.empty() )
      {
        soft_ref<const io::serializable> obj = m_pending_objs.back();
        m_pending_objs.pop_back();

        if ( !already_written(obj) )
          {
            m_buf << obj->obj_typename().c_str() << ' '
                  << m_id_map.get(obj->id()) << " := ";
            flatten_object(obj);
          }
      }

    m_buf.flush();
  }

  void asw_writer::flatten_object(soft_ref<const io::serializable> obj)
  {
  GVX_TRACE("asw_writer::flatten_object");

    // Objects are written in the following format:
    //
    // { ?<version id>? <attribute count>
    // ...attributes...
    // }
    //

    // Open the object's braces...
    m_buf << "{ ";

    //   ...write <version id> if it is nonzero...
    io::version_id serial_ver_id = obj->class_version_id();;
    if ( serial_ver_id > 0 )
      m_buf << 'v' << serial_ver_id << ' ';

    //   ...write the <attribute count>...
    m_buf << io::attrib_count(*obj) << '\n';

    //   ...write the object's <attributes>...
    obj->write_to(*this);

    mark_as_written(obj);

    //   ...and finally, close the object's braces.
    m_buf << '}' << '\n';
  }
}

shared_ptr<io::writer> io::make_asw_writer(std::ostream& os)
{
  return rutz::make_shared(new asw_writer(os));
}

shared_ptr<io::writer> io::make_asw_writer(const char* filename)
{
  return rutz::make_shared(new asw_writer(filename));
}

#endif // !GROOVX_IO_ASCIISTREAMWRITER_CC_UTC20050626084021_DEFINED
