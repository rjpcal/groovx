/** @file io/reader.h abstract base class for reading serializable
    io::serializable objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun  7 12:46:08 1999
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

#ifndef GROOVX_IO_READER_H_UTC20050626084021_DEFINED
#define GROOVX_IO_READER_H_UTC20050626084021_DEFINED

#include "io/io.h"

#include "rutz/fileposition.h"
#include "rutz/fstring.h"

namespace rutz
{
  struct byte_array;
  class value;
}

namespace nub
{
  template <class T> class ref;
  template <class T> class soft_ref;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c io::reader provides the interface that \c io objects use to
 * restore their state in a \c read_from() implementation. It provides
 * the inverse operations to those in \c io::writer. To reconstruct an
 * object tree, a client should call \c read_root() on the root object
 * in that tree. Subclasses of \c io::reader will implement this
 * interface using different back ends for the actual storage medium,
 * for example, an ASCII text file, an on-screen dialog box, a
 * relational database, etc.
 *
 **/
///////////////////////////////////////////////////////////////////////

class io::reader
{
public:

  /// Virtual destructor allows correct destruction of subclasses.
  virtual ~reader() noexcept;

  /** A convenience function to ensure that the current serial version
      is no less than the \a lowest_supported_version. Returns the
      actual version. If this test fails, an exception will be
      thrown. */
  io::version_id ensure_version_id(const char* name,
                                   io::version_id lowest_supported_version,
                                   const char* msg,
                                   const rutz::file_pos& pos);

  /** Returns the serialization version id that was stored with the
      object currently being read. */
  virtual io::version_id input_version_id() = 0;

  /// Read the \c char attribute associated with the tag \a name.
  virtual char read_char(const rutz::fstring& name) = 0;

  /// Read the \c int attribute associated with the tag \a name.
  virtual int read_int(const rutz::fstring& name) = 0;

  /// Read the \c bool attribute associated with the tag \a name.
  virtual bool read_bool(const rutz::fstring& name) = 0;

  /// Read the \c double attribute associated with the tag \a name.
  virtual double read_double(const rutz::fstring& name) = 0;

  /// Read the \c rutz::value attribute associated with the tag \a name.
  virtual void read_value_obj(const rutz::fstring& name,
                            rutz::value& v) = 0;

  /// Read the raw data array associated with the tag \a name.
  virtual rutz::byte_array read_byte_array(const rutz::fstring& name) = 0;

  /** @name Overloaded read functions

      These functions offer a set of overloads to provide compile-time
      polymorphism so that any supported type (any basic type, or
      rutz::value, a string type) or can be read with the same source
      code.
  */
  //@{

  void read_value(const rutz::fstring& name, char& ret)
  { ret = read_char(name); }

  void read_value(const rutz::fstring& name, int& ret)
  { ret = read_int(name); }

  void read_value(const rutz::fstring& name, unsigned int& ret)
  { ret = (unsigned int)(read_int(name)); }

  void read_value(const rutz::fstring& name, unsigned long& ret)
  { ret = (unsigned long)(read_int(name)); }

  void read_value(const rutz::fstring& name, bool& ret)
  { ret = read_bool(name); }

  void read_value(const rutz::fstring& name, float& ret)
  { double d = read_double(name); ret = float(d); }

  void read_value(const rutz::fstring& name, double& ret)
  { ret = read_double(name); }

  void read_value(const rutz::fstring& name, rutz::fstring& ret)
  { ret = read_string_impl(name); }

  void read_value(const rutz::fstring& name, rutz::value& ret)
  { read_value_obj(name, ret); }

  //@}

  /** Get a \c nub::ref associated with the tag \a name. A new object
      of the appropriate type will be created and inserted into the \c
      nub::objectdb, if necessary. */
  virtual nub::ref<io::serializable> read_object(const rutz::fstring& name) = 0;

  /** Get a \c soft_ref associated with the tag \a name. If no such
      object exists, a null object is returned; otherwise, a new
      object of the appropriate type will be created and inserted into
      the \c nub::objectdb, if necessary. */
  virtual nub::soft_ref<io::serializable> read_weak_object(const rutz::fstring& name) = 0;

  /** Restore the state of the io object \a obj, associated with the
      tag \a name. The io::reader will not create a new object, but
      will use the io* provided here. */
  virtual void read_owned_object(const rutz::fstring& name,
                               nub::ref<io::serializable> obj) = 0;

  /** Read the named base class into the io object \a obj, which
      should be arranged to point or refer to the appropriate base
      class part of the object. In particular, \a obj's virtual
      functions must NOT call the fully derived versions. This effect
      can be best accomplished with an \c io::proxy. */
  virtual void read_base_class(const rutz::fstring& base_class_name,
                               nub::ref<io::serializable> base_part) = 0;

  /** Restore an entire object hierarchy, starting with the root
      object. If \a root is non-null, the function will use \a root as
      the root object. Otherwise the function will create a new root
      object. In either case, the function returns the object that was
      actually used as the root object. */
  virtual nub::ref<io::serializable> read_root(io::serializable* root=0) = 0;

protected:
  /// Read the string attribute associated with the tag \a name.
  virtual rutz::fstring read_string_impl(const rutz::fstring& name) = 0;

  /// Base64 implementation of read_byte_array() for use by subclasses.
  rutz::byte_array default_read_byte_array(const rutz::fstring& name);
};

#endif // !GROOVX_IO_READER_H_UTC20050626084021_DEFINED
