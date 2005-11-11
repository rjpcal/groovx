/** @file io/io.h abstract base class for serializable objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jan  4 08:00:00 1999
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

#ifndef GROOVX_IO_IO_H_UTC20050626084021_DEFINED
#define GROOVX_IO_IO_H_UTC20050626084021_DEFINED

#include "nub/object.h"

namespace rutz
{
  class fstring;
}

namespace io
{
  class serializable;
  class reader;
  class writer;

  /// This type is used for verion ids during the read+write process.
  typedef signed long version_id;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * The io::serializable class defines the abstract interface for
 * object persistence. Classes which need these facilities should
 * inherit virtually from io::serializable. io::serializable is
 * reference counted (by subclassing nub::object), providing automatic
 * memory management when io::serializable's are managed with a smart
 * pointer that manages the reference count, such as nub::ref or
 * nub::soft_ref.
 *
 **/
///////////////////////////////////////////////////////////////////////

class io::serializable : public virtual nub::object
{
private:
  serializable(const serializable&);
  serializable& operator=(const serializable&);

protected:
  /// Default constructor.
  serializable() throw();

  /// Virtual destructor to ensure correct destruction of subclasses
  virtual ~serializable() GVX_DTOR_NOTHROW;

public:
  /** Subclasses implement this method to save the object's state via
      the generic interface provided by \c io::reader. Parsing the
      format of the input is handled by the \c io::reader, so
      implementors of \c read_from() of don't need to deal with
      formatting. */
  virtual void read_from(io::reader& reader) = 0;

  /** Subclasses implement this method to restore the object's state
      via the generic interface provided by \c io::writer. Formatting
      the output is handled by the \c io::writer, so implementors of
      \c write_to() of don't need to deal with formatting. */
  virtual void write_to(io::writer& writer) const = 0;

  /** Returns the number of attributes that are written in the
      object's \c write_to() function. The default implementation
      simply calls write_to() with a dummy writer and counts how many
      attributes are written. However, this is somewhat inefficient
      since it defers the counting to runtime when the number may in
      fact be known at compile time. Thus subclasses may wish to
      override this function to return a compile-time constant. */
  virtual unsigned int attrib_count() const;

  /** Returns a serialization version id for the class. The default
      implementation returns zero. Classes should override this when
      they make a change that requires a change to their serialization
      protocol. Overriding versions of this function should follow the
      convention that a higher id refers to a later version of the
      class. Implementations of \c io::reader and \c io::writer will
      provide a way for a class to store and retrieve the
      serialization version of an object. */
  virtual io::version_id class_version_id() const;
};


static const char vcid_groovx_io_io_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_IO_H_UTC20050626084021_DEFINED
