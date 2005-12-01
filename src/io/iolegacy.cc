/** @file io/iolegacy.cc io::reader and io::writer subclasses for the
    "legacy" file format */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Sep 27 08:40:04 2000
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

#ifndef GROOVX_IO_IOLEGACY_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_IOLEGACY_CC_UTC20050626084021_DEFINED

#include "io/iolegacy.h"

#include "nub/objmgr.h"
#include "nub/ref.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/value.h"

#include <cctype>
#include <cstring>
#include <iostream>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

using nub::ref;
using nub::soft_ref;

///////////////////////////////////////////////////////////////////////
//
// legacy_reader::impl definition
//
///////////////////////////////////////////////////////////////////////

class io::legacy_reader::impl
{
  impl(const impl&);
  impl& operator=(const impl&);

public:

  io::legacy_reader*  m_owner;
  std::istream&       m_stream;
  int                 m_legacy_version_id;

  impl(io::legacy_reader* owner, std::istream& is) :
    m_owner(owner),
    m_stream(is),
    m_legacy_version_id(0)
  {}

  void throw_if_error(const char* type, const rutz::file_pos& pos)
  {
    if (m_stream.fail())
      {
        dbg_print(3, "throw_if_error for"); dbg_eval_nl(3, type);
        throw rutz::error(type, pos);
      }
  }

  void throw_if_error(const fstring& type, const rutz::file_pos& pos)
  {
    throw_if_error(type.c_str(), pos);
  }

  void read_typename(const fstring& correct_name)
  {
    GVX_TRACE("io::legacy_reader::impl::read_typename(fstring)");

    fstring name;
    m_stream >> name;

    if (! name.equals(correct_name))
      {
        // If we got here, then none of the substrings matched so we must
        // raise an exception.
        throw rutz::error(rutz::cat("couldn't read typename for ",
                                    correct_name.c_str()), SRC_POS);
      }
  }

  // An override for when we have two alternative names
  void read_typename(const fstring& correct_name, const fstring& alt_name)
  {
    GVX_TRACE("io::legacy_reader::impl::read_typename(fstring, fstring)");

    fstring name;
    m_stream >> name;

    if ( !name.equals(correct_name) && !name.equals(alt_name) )
      {
        // If we got here, then none of the substrings matched so we must
        // raise an exception.
        throw rutz::error(rutz::cat("couldn't read typename for ",
                                    correct_name.c_str()), SRC_POS);
      }
  }

  int get_legacy_version_id()
  {
    GVX_TRACE("io::legacy_reader::impl::get_legacy_version_id");
    while ( isspace(m_stream.peek()) )
      { m_stream.get(); }

    int version = -1;

    if (m_stream.peek() == '@')
      {
        int c = m_stream.get();
        GVX_ASSERT(c == '@');

        m_stream >> version;
        dbg_eval_nl(3, version);
        throw_if_error("version id", SRC_POS);
      }
    else
      {
        throw rutz::error("missing legacy version id", SRC_POS);
      }

    return version;
  }

  void grab_left_brace()
  {
    char brace;
    m_stream >> brace;
    if (brace != '{')
      {
        dbg_print_nl(3, "grab_left_brace failed");
        throw rutz::error("missing left-brace", SRC_POS);
      }
  }

  void grab_right_brace()
  {
    char brace;
    m_stream >> brace;
    if (brace != '}')
      {
        dbg_print_nl(3, "grab_right_brace failed");
        throw rutz::error("missing right-brace", SRC_POS);
      }
  }

  void inflate_object(const fstring& name, ref<io::serializable> obj)
  {
    GVX_TRACE("io::legacy_reader::impl::inflate_object");

    dbg_eval_nl(3, name);

    m_legacy_version_id = get_legacy_version_id();
    if (m_legacy_version_id != -1)
      {
        grab_left_brace();
        obj->read_from(*m_owner);
        grab_right_brace();
      }

    throw_if_error(name, SRC_POS);
  }
};

///////////////////////////////////////////////////////////////////////
//
// legacy_reader member definitions
//
///////////////////////////////////////////////////////////////////////

io::legacy_reader::legacy_reader(std::istream& is) :
  rep(new impl(this, is))
{
GVX_TRACE("io::legacy_reader::legacy_reader");
}

io::legacy_reader::~legacy_reader() throw()
{
GVX_TRACE("io::legacy_reader::~legacy_reader");
  delete rep;
}

io::version_id io::legacy_reader::input_version_id()
{
GVX_TRACE("io::legacy_reader::input_version_id");
  dbg_eval_nl(3, rep->m_legacy_version_id);
  return rep->m_legacy_version_id;
}

char io::legacy_reader::read_char(const fstring& name)
{
GVX_TRACE("io::legacy_reader::read_char");
  dbg_eval(3, name);
  char val;
  rep->m_stream >> val;   dbg_eval_nl(3, val);
  rep->throw_if_error(name, SRC_POS);
  return val;
}

int io::legacy_reader::read_int(const fstring& name)
{
GVX_TRACE("io::legacy_reader::read_int");
  dbg_eval(3, name);
  int val;
  rep->m_stream >> val;   dbg_eval_nl(3, val);
  rep->throw_if_error(name, SRC_POS);
  return val;
}

bool io::legacy_reader::read_bool(const fstring& name)
{
GVX_TRACE("io::legacy_reader::read_bool");
  dbg_eval(3, name);
  int val;
  rep->m_stream >> val;   dbg_eval_nl(3, val);
  rep->throw_if_error(name, SRC_POS);
  return bool(val);
}

double io::legacy_reader::read_double(const fstring& name)
{
GVX_TRACE("io::legacy_reader::read_double");
  dbg_eval(3, name);
  double val;
  rep->m_stream >> val;   dbg_eval_nl(3, val);
  rep->throw_if_error(name, SRC_POS);
  return val;
}

fstring io::legacy_reader::read_string_impl(const fstring& name)
{
GVX_TRACE("io::legacy_reader::read_string_impl");
  dbg_eval_nl(3, name);

  int numchars = 0;
  rep->m_stream >> numchars;

  rep->throw_if_error(name, SRC_POS);

  if (numchars < 0)
    {
      throw rutz::error("legacy_reader::read_string_impl "
                        "saw negative character count", SRC_POS);
    }

  int c = rep->m_stream.get();
  if (c != ' ')
    {
      throw rutz::error("legacy_reader::read_string_impl "
                        "did not have whitespace after character count", SRC_POS);
    }

  fstring new_string;
  new_string.readsome(rep->m_stream, static_cast<unsigned int>(numchars));

  rep->throw_if_error(name, SRC_POS);

  dbg_eval_nl(3, new_string);

  return new_string;
}

void io::legacy_reader::read_value_obj(const fstring& name, rutz::value& v)
{
GVX_TRACE("io::legacy_reader::read_value_obj");
  dbg_eval_nl(3, name);
  v.scan_from(rep->m_stream);
  rep->throw_if_error(name, SRC_POS);
}

ref<io::serializable>
io::legacy_reader::read_object(const fstring& name)
{
GVX_TRACE("io::legacy_reader::read_object");
  return ref<io::serializable>(read_weak_object(name));
}

soft_ref<io::serializable>
io::legacy_reader::read_weak_object(const fstring& name)
{
GVX_TRACE("io::legacy_reader::read_weak_object");
  dbg_eval(3, name);
  fstring type;
  rep->m_stream >> type; dbg_eval(3, type);

  if (type == "NULL")
    {
      return soft_ref<io::serializable>();
    }

  ref<io::serializable> obj(nub::obj_mgr::new_typed_obj<io::serializable>(type));
  dbg_eval_nl(3, obj->obj_typename());

  rep->inflate_object(name, obj);

  return obj;
}

void io::legacy_reader::read_owned_object(const fstring& name,
                                       ref<io::serializable> obj)
{
GVX_TRACE("io::legacy_reader::read_owned_object");

  rep->read_typename(obj->obj_typename());
  rep->inflate_object(name, obj);
}

void io::legacy_reader::read_base_class(const fstring& base_class_name,
                                        ref<io::serializable> base_part)
{
GVX_TRACE("io::legacy_reader::read_base_class");

  // For backward-compatibility, we allow the typename to match either
  // the real typename of the base part, or the descriptive name given
  // to the base class.
  rep->read_typename(base_part->obj_typename(), base_class_name);
  rep->inflate_object(base_class_name, base_part);
}

ref<io::serializable> io::legacy_reader::read_root(io::serializable* given_root)
{
GVX_TRACE("io::legacy_reader::read_root");
  if (given_root == 0)
    {
      return read_object("root_object");
    }

  dbg_eval_nl(3, given_root->obj_typename());

  ref<io::serializable> root(given_root);
  read_owned_object("root_object", root);

  return root;
}

///////////////////////////////////////////////////////////////////////
//
// legacy_writer::impl definition
//
///////////////////////////////////////////////////////////////////////

class io::legacy_writer::impl
{
private:
  impl(const impl&);
  impl& operator=(const impl&);

public:
  impl(io::legacy_writer* owner, std::ostream& os, bool write_bases) :
    m_owner(owner),
    m_stream(os),
    m_write_bases(write_bases),
    m_field_sep(' '),
    m_indent_level(0),
    m_needs_newline(false),
    m_needs_whitespace(false),
    m_is_beginning(true),
    m_do_pretty_print(true)
  {}

  void throw_if_error(const char* type, const rutz::file_pos& pos)
  {
    if (m_stream.fail())
      {
        dbg_print(3, "throw_if_error for"); dbg_eval_nl(3, type);
        throw rutz::error(type, pos);
      }
  }

  io::legacy_writer*  m_owner;
private:
  std::ostream&       m_stream;
public:
  const bool          m_write_bases;
  const char          m_field_sep;
  int                 m_indent_level;
  bool                m_needs_newline;
  bool                m_needs_whitespace;
  bool                m_is_beginning;
  bool                m_do_pretty_print;

  std::ostream& stream()
    {
      flush_whitespace();
      m_is_beginning = false;
      return m_stream;
    }

  void flush_whitespace()
    {
      update_newline();
      update_whitespace();
    }

private:
  class Indenter
  {
  private:
    impl* m_owner;

    Indenter(const Indenter&);
    Indenter& operator=(const Indenter&);

  public:
    Indenter(impl* impl) : m_owner(impl) { ++(m_owner->m_indent_level); }
    ~Indenter() { --(m_owner->m_indent_level); }
  };

  void do_newline_and_tabs()
    {
      m_stream << '\n';
      for (int i = 0; i < m_indent_level; ++i)
        m_stream << '\t';
    }

  void do_whitespace()
    {
      if (m_do_pretty_print)
        do_newline_and_tabs();
      else
        m_stream << ' ';
    }

  void update_newline()
    {
      if (m_needs_newline)
        {
          do_newline_and_tabs();
          no_newline_needed();
          no_whitespace_needed();
        }
    }

  void update_whitespace()
    {
      if (m_needs_whitespace)
        {
          do_whitespace();
          no_whitespace_needed();
        }
    }

public:
  void use_pretty_print(bool yes) { m_do_pretty_print = yes; }

  void request_newline() { if (!m_is_beginning) m_needs_newline = true; }
  void request_whitespace() { if (!m_is_beginning) m_needs_whitespace = true; }
  void no_newline_needed() { m_needs_newline = false; }
  void no_whitespace_needed() { m_needs_whitespace = false; }

  void flatten_object(const char* obj_name,
                      soft_ref<const io::serializable> obj,
                      bool stub_out = false)
  {
    if (m_indent_level > 0)
      request_whitespace();
    else
      request_newline();

    if ( !(obj.is_valid()) )
      {
        stream() << "NULL" << m_field_sep;
        throw_if_error(obj_name, SRC_POS);
        return;
      }

    GVX_ASSERT(obj.is_valid());

    stream() << obj->obj_typename() << m_field_sep;
    throw_if_error(obj->obj_typename().c_str(), SRC_POS);

    stream() << '@';

    if (stub_out)
      {
        stream() << "-1 ";
      }
    else
      {
        stream() << obj->class_version_id() << " {";
        {
          Indenter indent(this);
          request_whitespace();
          obj->write_to(*m_owner);
        }
        request_whitespace();
        stream() << "}";
      }

    if (m_indent_level > 0)
      request_whitespace();
    else
      request_newline();

    throw_if_error(obj_name, SRC_POS);
  }
};

///////////////////////////////////////////////////////////////////////
//
// legacy_writer member definitions
//
///////////////////////////////////////////////////////////////////////


io::legacy_writer::legacy_writer(std::ostream& os, bool write_bases) :
  rep(new impl(this, os, write_bases))
{
GVX_TRACE("io::legacy_writer::legacy_writer");
}

io::legacy_writer::~legacy_writer() throw()
{
GVX_TRACE("io::legacy_writer::~legacy_writer");
  rep->flush_whitespace();
  delete rep;
}

void io::legacy_writer::use_pretty_print(bool yes)
{
GVX_TRACE("io::legacy_writer::use_pretty_print");
  rep->use_pretty_print(yes);
}

void io::legacy_writer::write_char(const char* name, char val)
{
GVX_TRACE("io::legacy_writer::write_char");
  rep->stream() << val << rep->m_field_sep;
  rep->throw_if_error(name, SRC_POS);
}

void io::legacy_writer::write_int(const char* name, int val)
{
GVX_TRACE("io::legacy_writer::write_int");
  rep->stream() << val << rep->m_field_sep;
  rep->throw_if_error(name, SRC_POS);
}

void io::legacy_writer::write_bool(const char* name, bool val)
{
GVX_TRACE("io::legacy_writer::write_bool");
  rep->stream() << val << rep->m_field_sep;
  rep->throw_if_error(name, SRC_POS);
}

void io::legacy_writer::write_double(const char* name, double val)
{
GVX_TRACE("io::legacy_writer::write_double");
  rep->stream() << val << rep->m_field_sep;
  rep->throw_if_error(name, SRC_POS);
}

void io::legacy_writer::write_cstring(const char* name, const char* val)
{
GVX_TRACE("io::legacy_writer::write_cstring");

  rep->stream() << strlen(val) << " " << val << rep->m_field_sep;

  rep->throw_if_error(name, SRC_POS);
}

void io::legacy_writer::write_value_obj(const char* name,
                                     const rutz::value& v)
{
GVX_TRACE("io::legacy_writer::write_value_obj");
  v.print_to(rep->stream());
  rep->stream() << rep->m_field_sep;
  rep->throw_if_error(name, SRC_POS);
}

void io::legacy_writer::write_byte_array(const char* name,
                                         const unsigned char* data,
                                         unsigned int length)
{
GVX_TRACE("io::legacy_writer::write_byte_array");
  default_write_byte_array(name, data, length);
}

void io::legacy_writer::write_object(const char* name,
                                     soft_ref<const io::serializable> obj)
{
GVX_TRACE("io::legacy_writer::write_object");

  rep->flatten_object(name, obj);
}

void io::legacy_writer::write_owned_object(const char* name,
                                           ref<const io::serializable> obj)
{
GVX_TRACE("io::legacy_writer::write_owned_object");

  rep->flatten_object(name, obj);
}

void io::legacy_writer::write_base_class(const char* base_class_name,
                                         ref<const io::serializable> base_part)
{
GVX_TRACE("io::legacy_writer::write_base_class");
  if (rep->m_write_bases)
    {
      rep->flatten_object(base_class_name, base_part);
    }
  else
    {
      rep->flatten_object(base_class_name, base_part, true);
    }
}

void io::legacy_writer::write_root(const io::serializable* root)
{
GVX_TRACE("io::legacy_writer::write_root");

  rep->flatten_object
    ("root_object", soft_ref<io::serializable>(const_cast<io::serializable*>(root),
                                               nub::STRONG,
                                               nub::PRIVATE));
}

static const char vcid_groovx_io_iolegacy_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_IOLEGACY_CC_UTC20050626084021_DEFINED
