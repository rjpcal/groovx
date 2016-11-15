/** @file tcl-io/objreader.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov  8 15:37:35 2005
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

#ifndef GROOVX_TCL_IO_OBJREADER_H_UTC20051108233735_DEFINED
#define GROOVX_TCL_IO_OBJREADER_H_UTC20051108233735_DEFINED

#include "io/reader.h"
#include "tcl/obj.h"

namespace tcl
{
  class obj_reader;
}

class tcl::obj_reader : public io::reader
{
public:

  /// Construct with a tcl::obj to read from.
  obj_reader(tcl::obj x);

  /// Virtual destructor allows correct destruction of subclasses.
  virtual ~obj_reader() noexcept;

  /** Returns the serialization version id that was stored with the
      object currently being read. */
  virtual io::version_id input_version_id() override;

  /// Read the \c char attribute associated with the tag \a name.
  virtual char read_char(const rutz::fstring& name) override;

  /// Read the \c int attribute associated with the tag \a name.
  virtual int read_int(const rutz::fstring& name) override;

  /// Read the \c bool attribute associated with the tag \a name.
  virtual bool read_bool(const rutz::fstring& name) override;

  /// Read the \c double attribute associated with the tag \a name.
  virtual double read_double(const rutz::fstring& name) override;

  /// Read the \c rutz::value attribute associated with the tag \a name.
  virtual void read_value_obj(const rutz::fstring& name,
                            rutz::value& v) override;

  /// Read the raw data array associated with the tag \a name.
  virtual rutz::byte_array read_byte_array(const rutz::fstring& name) override;

  /** Get a \c nub::ref associated with the tag \a name. A new object
      of the appropriate type will be created and inserted into the \c
      nub::objectdb, if necessary. */
  virtual nub::ref<io::serializable> read_object(const rutz::fstring& name) override;

  /** Get a \c soft_ref associated with the tag \a name. If no such
      object exists, a null object is returned; otherwise, a new
      object of the appropriate type will be created and inserted into
      the \c nub::objectdb, if necessary. */
  virtual nub::soft_ref<io::serializable> read_weak_object(const rutz::fstring& name) override;

  /** Restore the state of the io object \a obj, associated with the
      tag \a name. The io::reader will not create a new object, but
      will use the io* provided here. */
  virtual void read_owned_object(const rutz::fstring& name,
                                 nub::ref<io::serializable> obj) override;

  /** Read the named base class into the io object \a obj, which
      should be arranged to point or refer to the appropriate base
      class part of the object. In particular, \a obj's virtual
      functions must NOT call the fully derived versions. This effect
      can be best accomplished with an \c io::proxy. */
  virtual void read_base_class(const rutz::fstring& base_class_name,
                               nub::ref<io::serializable> base_part) override;

  /** Restore an entire object hierarchy, starting with the root
      object. If \a root is non-null, the function will use \a root as
      the root object. Otherwise the function will create a new root
      object. In either case, the function returns the object that was
      actually used as the root object. */
  virtual nub::ref<io::serializable> read_root(io::serializable* root=0) override;

protected:
  /// Read the string attribute associated with the tag \a name.
  virtual rutz::fstring read_string_impl(const rutz::fstring& name) override;

private:
  const tcl::obj m_obj;
};

#endif // !GROOVX_TCL_IO_OBJREADER_H_UTC20051108233735DEFINED
