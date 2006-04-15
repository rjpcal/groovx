/** @file io/iolegacy.h io::reader and io::writer subclasses for the
    "legacy" file format */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Sep 26 18:47:31 2000
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

#ifndef GROOVX_IO_IOLEGACY_H_UTC20050626084021_DEFINED
#define GROOVX_IO_IOLEGACY_H_UTC20050626084021_DEFINED

#include "io/io.h"
#include "io/reader.h"
#include "io/writer.h"

#include <iosfwd>

namespace io
{
  class legacy_reader;
  class legacy_writer;
}

// ########################################################

/// Implements the io::reader interface using the "legacy" data format.

class io::legacy_reader : public io::reader
{
public:
  legacy_reader(std::istream& is);
  virtual ~legacy_reader() throw();

  virtual io::version_id input_version_id();

  virtual char read_char(const rutz::fstring& name);
  virtual int read_int(const rutz::fstring& name);
  virtual bool read_bool(const rutz::fstring& name);
  virtual double read_double(const rutz::fstring& name);
  virtual void read_value_obj(const rutz::fstring& name, rutz::value& v);

  virtual void read_byte_array(const rutz::fstring& name, rutz::byte_array& data)
  { default_read_byte_array(name, data); }

  virtual nub::ref    <io::serializable>      read_object(const rutz::fstring& name);
  virtual nub::soft_ref<io::serializable> read_weak_object(const rutz::fstring& name);

  virtual void read_owned_object(const rutz::fstring& name,
                               nub::ref<io::serializable> obj);

  virtual void read_base_class(const rutz::fstring& base_class_name,
                               nub::ref<io::serializable> base_part);

  virtual nub::ref<io::serializable> read_root(io::serializable* root=0);

protected:
  virtual rutz::fstring read_string_impl(const rutz::fstring& name);

private:
  legacy_reader(const legacy_reader&);
  legacy_reader& operator=(const legacy_reader&);

  class impl;
  friend class impl;

  impl* const rep;
};


// ########################################################

/// Implements the io::writer interface using the "legacy" data format.

class io::legacy_writer : public io::writer
{
public:
  legacy_writer(std::ostream& os, bool write_bases=true);
  virtual ~legacy_writer() throw();

  void use_pretty_print(bool yes=true);

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
  legacy_writer(const legacy_writer&);
  legacy_writer& operator=(const legacy_writer&);

  class impl;
  friend class impl;

  impl* const rep;
};

static const char __attribute__((used)) vcid_groovx_io_iolegacy_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_IOLEGACY_H_UTC20050626084021_DEFINED
