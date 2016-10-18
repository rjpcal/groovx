/** @file tcl-io/objwriter.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov  8 15:38:28 2005
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

#ifndef GROOVX_TCL_IO_OBJWRITER_H_UTC20051108233828_DEFINED
#define GROOVX_TCL_IO_OBJWRITER_H_UTC20051108233828_DEFINED

#include "io/writer.h"
#include "tcl/obj.h"

namespace tcl
{
  class obj_writer;
}

class tcl::obj_writer : public io::writer
{
public:
  obj_writer();

  /// Virtual destructor allows correct destruction of subclasses.
  virtual ~obj_writer() noexcept;

  tcl::obj get_obj() const { return m_obj; }

  /// Store the \c char attribute \a val in association with the tag \a name.
  virtual void write_char(const char* name, char val);

  /// Store the \c int attribute \a val in association with the tag \a name.
  virtual void write_int(const char* name, int val);

  /// Store the \c bool attribute \a val in association with the tag \a name.
  virtual void write_bool(const char* name, bool val);

  /// Store the \c double attribute \a val in association with the tag \a name.
  virtual void write_double(const char* name, double val);

  /// Store the \c rutz::value attribute \a val in association with the tag \a name.
  virtual void write_value_obj(const char* name, const rutz::value& v);

  /// Store the raw data array in association with the tag \a name.
  virtual void write_byte_array(const char* name,
                            const unsigned char* data,
                            unsigned int length);

  /// Store the \c io object \a val in association with the tag \a name.
  virtual void write_object(const char* name,
                            nub::soft_ref<const io::serializable> obj);

  /** Store the owned \c io object \a obj in association with the tag
      \a name. This function should only be used if \a obj is \b owned
      by the storing object; no other objects should reference \a
      obj. This allows the \c writer subclass to implement the storage
      of an owned object as a contained object. */
  virtual void write_owned_object(const char* name,
                                  nub::ref<const io::serializable> obj);

  /** Write the named base class using the io object \a obj, which
      should be arranged to point or refer to the appropriate base
      class part of the object. In particular, \a obj's virtual
      functions must NOT call the fully derived versions. This effect
      can be best accomplished with an \c io::proxy. */
  virtual void write_base_class(const char* base_class_name,
                                nub::ref<const io::serializable> base_part);

  /** Store an entire object hierarchy, starting with the root object
      \a root. All objects and values referenced by \a root will be
      stored recursively, until there are no more remaining
      references. */
  virtual void write_root(const io::serializable* root);

protected:
  /// Store the C-style string (\c char*) attribute \a val in association with the tag \a name.
  virtual void write_cstring(const char* name, const char* val);

private:
  tcl::obj m_obj;
};

#endif // !GROOVX_TCL_IO_OBJWRITER_H_UTC20051108233828DEFINED
