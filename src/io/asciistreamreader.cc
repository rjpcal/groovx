/** @file io/asciistreamreader.cc read io::serializable objects in the ASW
    format */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun  7 12:54:55 1999
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

#ifndef GROOVX_IO_ASCIISTREAMREADER_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_ASCIISTREAMREADER_CC_UTC20050626084021_DEFINED

#include "io/asciistreamreader.h"

#include "io/io.h"
#include "io/reader.h"
#include "io/readattribmap.h"
#include "io/readobjectmap.h"

#include "nub/ref.h"

#include "rutz/cstrstream.h"
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/gzstreambuf.h"
#include "rutz/sfmt.h"
#include "rutz/shared_ptr.h"
#include "rutz/value.h"

#include <istream>
#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;
using rutz::shared_ptr;

using nub::ref;
using nub::soft_ref;

using io::attrib_map;

namespace
{
  void throw_attr_error(const fstring& attrib_name,
                        const fstring& attrib_value,
                        const rutz::file_pos& pos)
  {
    throw rutz::error(rutz::sfmt("error reading attribute '%s' "
                                 "with value '%s'",
                                 attrib_name.c_str(),
                                 attrib_value.c_str()),
                      pos);
  }

  fstring read_and_unescape(std::istream& is)
  {
  GVX_TRACE("<asciistreamreader.cc>::read_and_unescape");

    static const char STRING_ENDER = '^';

    static std::vector<char> buffer(4096);

    buffer.resize(0);

    int brace_level = 0;

    int ch = 0;

    while ( (ch = is.get()) != EOF &&
            !(brace_level == 0 && ch == STRING_ENDER) )
      {
        // We only substitute in the escape sequence if we are reading
        // at the zero-th brace level; otherwise, we leave the escape
        // sequence in since it will eventually be parsed when the
        // brace-nested object itself is parsed.
        if (ch != '\\' || brace_level > 0)
          {
            if (ch == '{') ++brace_level;
            if (ch == '}') --brace_level;
            buffer.push_back(char(ch));
            continue;
          }
        else
          {
            const int ch2 = is.get();

            if (ch2 == EOF || ch2 == STRING_ENDER)
              throw rutz::error("missing character "
                                "after trailing backslash", SRC_POS);

            switch (ch2)
              {
              case '\\': buffer.push_back('\\'); break;
              case 'c':  buffer.push_back('^');  break;
              case '{':  buffer.push_back('{');  break;
              case '}':  buffer.push_back('}');  break;

              default:
                buffer.push_back('\0');
                throw rutz::error
                  (rutz::sfmt("invalid escape character '%c' "
                              "with buffer contents: %s",
                              ch2, &buffer[0]),
                   SRC_POS);
                break;
              }
          }
      } // while loop

    return fstring(rutz::char_range(&buffer[0], buffer.size()));
  }

  class asw_reader : public io::reader
  {
  public:
    asw_reader(std::istream& is);
    asw_reader(const char* filename);

    virtual ~asw_reader() throw();

    virtual io::version_id input_version_id();

    virtual char read_char(const fstring& name);
    virtual int read_int(const fstring& name);
    virtual bool read_bool(const fstring& name);
    virtual double read_double(const fstring& name);
    virtual void read_value_obj(const fstring& name, rutz::value& v);

    virtual void read_byte_array(const fstring& name, rutz::byte_array& data)
    { default_read_byte_array(name, data); }

    virtual nub::ref<io::serializable> read_object(const fstring& name);
    virtual nub::soft_ref<io::serializable>
    read_weak_object(const fstring& name);

    virtual void read_owned_object(const fstring& name,
                                   nub::ref<io::serializable> obj);
    virtual void read_base_class(const fstring& base_class_name,
                                 nub::ref<io::serializable> base_part);

    virtual nub::ref<io::serializable> read_root(io::serializable* root=0);

  protected:
    virtual fstring read_string_impl(const fstring& name);

  private:
    shared_ptr<std::istream>              m_owned_stream;
    std::istream&                         m_buf;
    io::object_map                        m_objects;
    std::vector<shared_ptr<attrib_map> >  m_attribs;

    attrib_map& current_attribs()
    {
      if ( m_attribs.empty() )
        throw rutz::error("attempted to read attribute "
                          "when no attribute map was active", SRC_POS);
      return *(m_attribs.back());
    }

    void inflate_object(std::istream& buf,
                        const fstring& obj_tag,
                        ref<io::serializable> obj);

    template <class T>
    T read_basic_type(const fstring& name)
    {
      dbg_eval_nl(3, name);

      attrib_map::attrib a = current_attribs().get(name);
      rutz::icstrstream ist(a.value.c_str());

      T return_val;
      ist >> return_val;
      dbg_eval(3, a.value); dbg_eval_nl(3, return_val);

      if (ist.fail())
        throw_attr_error(name, a.value, SRC_POS);

      return return_val;
    }
  };

  ///////////////////////////////////////////////////////////////////////
  //
  // asw_reader member definitions
  //
  ///////////////////////////////////////////////////////////////////////

  asw_reader::asw_reader(std::istream& is) :
    m_owned_stream(),
    m_buf(is),
    m_objects(),
    m_attribs()
  {
  GVX_TRACE("asw_reader::asw_reader");
  }

  asw_reader::asw_reader(const char* filename) :
    m_owned_stream(rutz::igzopen(filename)),
    m_buf(*m_owned_stream),
    m_objects(),
    m_attribs()
  {
  GVX_TRACE("asw_reader::asw_reader");
  }

  asw_reader::~asw_reader () throw()
  {
  GVX_TRACE("asw_reader::~asw_reader");
  }

  io::version_id asw_reader::input_version_id()
  {
  GVX_TRACE("asw_reader::input_version_id");
    return current_attribs().get_version_id();
  }

  char asw_reader::read_char(const fstring& name)
  {
  GVX_TRACE("asw_reader::read_char");
    return read_basic_type<char>(name);
  }

  int asw_reader::read_int(const fstring& name)
  {
  GVX_TRACE("asw_reader::read_int");
    return read_basic_type<int>(name);
  }

  bool asw_reader::read_bool(const fstring& name)
  {
  GVX_TRACE("asw_reader::read_bool");
    return bool(read_basic_type<int>(name));
  }

  double asw_reader::read_double(const fstring& name)
  {
  GVX_TRACE("asw_reader::read_double");
    return read_basic_type<double>(name);
  }

  fstring asw_reader::read_string_impl(const fstring& name)
  {
  GVX_TRACE("asw_reader::read_string_impl");
    dbg_eval_nl(3, name);

    attrib_map::attrib a = current_attribs().get(name);
    rutz::icstrstream ist(a.value.c_str());

    int len;
    ist >> len;                     dbg_eval_nl(3, len);
    ist.get(); // ignore one char of whitespace after the length

    if (len < 0)
      {
        throw rutz::error(rutz::sfmt("found a negative length "
                                     "for a string attribute: %d", len),
                          SRC_POS);
      }

    fstring new_string;
    new_string.readsome(ist, static_cast<unsigned int>(len));

    if (ist.fail())
      {
        throw_attr_error(name, a.value, SRC_POS);
      }

    dbg_eval(3, a.value); dbg_eval_nl(3, new_string);
    return new_string;
  }

  void asw_reader::read_value_obj(const fstring& name,
                                  rutz::value& v)
  {
  GVX_TRACE("asw_reader::read_value_obj");
    dbg_eval_nl(3, name);

    attrib_map::attrib a = current_attribs().get(name);

    v.set_string(a.value);
  }

  ref<io::serializable>
  asw_reader::read_object(const fstring& name)
  {
  GVX_TRACE("asw_reader::read_object");
    dbg_eval_nl(3, name);
    return ref<io::serializable>(read_weak_object(name));
  }

  soft_ref<io::serializable>
  asw_reader::read_weak_object(const fstring& name)
  {
  GVX_TRACE("asw_reader::read_weak_object");
    dbg_eval_nl(3, name);

    attrib_map::attrib attrib = current_attribs().get(name);

    rutz::icstrstream ist(attrib.value.c_str());
    nub::uid id;
    ist >> id;

    if (ist.fail())
      throw_attr_error(name, attrib.value, SRC_POS);

    if (id == 0) { return soft_ref<io::serializable>(); }

    // Return the object for this id, creating a new object if necessary:
    return m_objects.fetch_object(attrib.type, id);
  }

  void asw_reader::read_owned_object(const fstring& name,
                                     ref<io::serializable> obj)
  {
  GVX_TRACE("asw_reader::read_owned_object");
    dbg_eval_nl(3, name);

    attrib_map::attrib a = current_attribs().get(name);
    rutz::icstrstream ist(a.value.c_str());
    char bracket[16];

    ist >> bracket;

    inflate_object(ist, name, obj);

    ist >> bracket >> std::ws;
  }

  void asw_reader::read_base_class(const fstring& base_class_name,
                                   ref<io::serializable> base_part)
  {
  GVX_TRACE("asw_reader::read_base_class");
    dbg_eval_nl(3, base_class_name);
    read_owned_object(base_class_name, base_part);
  }

  ref<io::serializable> asw_reader::read_root(io::serializable* given_root)
  {
  GVX_TRACE("asw_reader::read_root");

    m_objects.clear();

    bool got_root = false;
    nub::uid rootid = 0;

    fstring type;
    fstring equal;
    fstring bracket;
    nub::uid id;

    while ( m_buf.peek() != EOF )
      {
        m_buf >> type >> id >> equal >> bracket;
        dbg_eval(3, type); dbg_eval_nl(3, id);

        if ( m_buf.fail() )
          {
            const fstring msg =
              rutz::sfmt("input failed while reading "
                         "typename and object id\n"
                         "\ttype: %s\n"
                         "\tid: %lu\n"
                         "\tequal: %s\n"
                         "\tbracket: %s",
                         type.c_str(), id, equal.c_str(), bracket.c_str());
            throw rutz::error(msg, SRC_POS);
          }

        if ( !got_root )
          {
            rootid = id;

            if (given_root != 0)
              m_objects.add_object_for_id
                (rootid, ref<io::serializable>(given_root));

            got_root = true;
          }

        ref<io::serializable> obj = m_objects.fetch_object(type, id);

        inflate_object(m_buf, type, obj);

        m_buf >> bracket >> std::ws;

        if ( m_buf.fail() )
          {
            throw rutz::error(rutz::sfmt("input failed "
                                         "while parsing ending bracket\n"
                                         "\tbracket: %s", bracket.c_str()),
                              SRC_POS);
          }
      }

    return m_objects.get_existing_object(rootid);
  }

  void asw_reader::inflate_object(std::istream& buf,
                                  const fstring& obj_tag,
                                  ref<io::serializable> obj)
  {
  GVX_TRACE("asw_reader::inflate_object");

    //
    // (1) read the object's attributes from the stream...
    //
    shared_ptr<attrib_map> attribs( new attrib_map(obj_tag) );

    // Skip all whitespace
    buf >> std::ws;

    io::version_id svid = 0;

    // Check if there is a version id in the stream
    if (buf.peek() == 'v')
      {
        int ch = buf.get();  GVX_ASSERT(ch == 'v');
        buf >> svid;
        if ( buf.fail() )
          throw rutz::error("input failed while reading "
                            "serialization version id", SRC_POS);
      }

    attribs->set_version_id(svid);

    // Get the attribute count
    int attrib_count;
    buf >> attrib_count;    dbg_eval_nl(3, attrib_count);

    if (attrib_count < 0)
      {
        throw rutz::error(rutz::sfmt("found a negative attribute count: %d",
                                     attrib_count), SRC_POS);
      }

    if ( buf.fail() )
      {
        throw rutz::error(rutz::sfmt("input failed while reading "
                                     "attribute count: %d", attrib_count),
                          SRC_POS);
      }

    // Loop and load all the attributes
    fstring type;
    fstring name;
    fstring equal;

    for (int i = 0; i < attrib_count; ++i)
      {
        buf >> type >> name >> equal;   dbg_eval(3, type); dbg_eval_nl(3, name);

        if ( buf.fail() )
          {
            const fstring msg =
              rutz::sfmt("input failed while reading "
                         "attribute type and name\n"
                         "\ttype: %s\n"
                         "\tname: %s\n"
                         "\tequal: %s",
                         type.c_str(), name.c_str(), equal.c_str());

            throw rutz::error(msg, SRC_POS);
          }

        attribs->add_attrib(name, type, read_and_unescape(buf));
      }

    m_attribs.push_back(attribs);

    //
    // (2) now the object can query us for its attributes...
    //
    obj->read_from(*this);

    m_attribs.pop_back();
  }
}

shared_ptr<io::reader> io::make_asw_reader(std::istream& os)
{
  return rutz::make_shared( new asw_reader(os) );
}

shared_ptr<io::reader> io::make_asw_reader(const char* filename)
{
  return rutz::make_shared( new asw_reader(filename) );
}

static const char __attribute__((used)) vcid_groovx_io_asciistreamreader_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_ASCIISTREAMREADER_CC_UTC20050626084021_DEFINED
