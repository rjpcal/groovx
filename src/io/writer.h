/** @file io/writer.h abstract base class for writing io::serializable
    objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun  7 12:49:49 1999
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

#ifndef GROOVX_IO_WRITER_H_UTC20050626084021_DEFINED
#define GROOVX_IO_WRITER_H_UTC20050626084021_DEFINED

#include "io/io.h"

#include "rutz/fileposition.h"
#include "rutz/fstring.h"

namespace rutz
{
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
 * \c io::writer provides the interface that \c io objects use to save
 * their state in a \c write_to() implementation. It provides the
 * inverse operations to those in \c io::reader. To save an object tree, a
 * client should call \c write_root() on the root object in that
 * tree. Subclasses of \c io::writer will implement this interface using
 * different back ends for the actual storage medium, for example, an
 * ASCII text file, an on-screen dialog box, a relational database,
 * etc.
 *
 **/
///////////////////////////////////////////////////////////////////////

class io::writer
{
public:

  /// Virtual destructor allows correct destruction of subclasses.
  virtual ~writer() throw();

  /** A convenience function to ensure that the current serial version
      is no less than the \a lowest_supported_version. Returns the \a
      actual_version. If this test fails, a write_version_error will
      be thrown.*/
  int ensure_output_version_id(const char* name,
                               io::version_id actual_version,
                               io::version_id lowest_supported_version,
                               const char* msg,
                               const rutz::file_pos& pos);

  /// Store the \c char attribute \a val in association with the tag \a name.
  virtual void write_char(const char* name, char val) = 0;

  /// Store the \c int attribute \a val in association with the tag \a name.
  virtual void write_int(const char* name, int val) = 0;

  /// Store the \c bool attribute \a val in association with the tag \a name.
  virtual void write_bool(const char* name, bool val) = 0;

  /// Store the \c double attribute \a val in association with the tag \a name.
  virtual void write_double(const char* name, double val) = 0;

  /// Store the \c rutz::value attribute \a val in association with the tag \a name.
  virtual void write_value_obj(const char* name, const rutz::value& v) = 0;

  /// Store the raw data array in association with the tag \a name.
  virtual void write_byte_array(const char* name,
                            const unsigned char* data,
                            unsigned int length) = 0;

  /** @name Overloaded write functions

      These functions offer a set of overloads to provide compile-time
      polymorphism so that any supported type (any basic type, or
      rutz::value, a string type) or can be written with the same
      source code.
  */
  //@{

  void write_value(const char* name, char val)
  { write_char(name, val); }

  void write_value(const char* name, int val)
  { write_int(name, val); }

  void write_value(const char* name, unsigned int val)
  { write_int(name, val); }

  void write_value(const char* name, unsigned long val)
  { write_int(name, val); }

  void write_value(const char* name, bool val)
  { write_bool(name, val); }

  void write_value(const char* name, double val)
  { write_double(name, val); }

  void write_value(const char* name, const rutz::fstring& val)
  { write_cstring(name, val.c_str()); }

  void write_value(const char* name, const rutz::value& v)
  { write_value_obj(name, v); }

  //@}

  /// Store the \c io object \a val in association with the tag \a name.
  virtual void write_object(const char* name,
                            nub::soft_ref<const io::serializable> obj) = 0;

  /** Store the owned \c io object \a obj in association with the tag
      \a name. This function should only be used if \a obj is \b owned
      by the storing object; no other objects should reference \a
      obj. This allows the \c writer subclass to implement the storage
      of an owned object as a contained object. */
  virtual void write_owned_object(const char* name,
                                  nub::ref<const io::serializable> obj) = 0;

  /** Write the named base class using the io object \a obj, which
      should be arranged to point or refer to the appropriate base
      class part of the object. In particular, \a obj's virtual
      functions must NOT call the fully derived versions. This effect
      can be best accomplished with an \c io::proxy. */
  virtual void write_base_class(const char* base_class_name,
                                nub::ref<const io::serializable> base_part) = 0;

  /** Store an entire object hierarchy, starting with the root object
      \a root. All objects and values referenced by \a root will be
      stored recursively, until there are no more remaining
      references. */
  virtual void write_root(const io::serializable* root) = 0;

protected:
  /// Store the C-style string (\c char*) attribute \a val in association with the tag \a name.
  virtual void write_cstring(const char* name, const char* val) = 0;


  /// Base64 implementation of write_byte_array() for use by subclasses.
  void default_write_byte_array(const char* name,
                                const unsigned char* data,
                                unsigned int length);
};

static const char __attribute__((used)) vcid_groovx_io_writer_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_WRITER_H_UTC20050626084021_DEFINED
