///////////////////////////////////////////////////////////////////////
//
// io.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
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

#ifndef IO_H_DEFINED
#define IO_H_DEFINED

#include "util/object.h"

#include "io/iodecls.h"

namespace rutz
{
  class fstring;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * The IO::IoObject class defines the abstract interface for object
 * persistence. Classes which need these facilities should inherit
 * virtually from IO::IoObject. IO::IoObject is reference counted (by
 * subclassing Util::Object), providing automatic memory management
 * when IO::IoObject's are managed with a smart pointer that manages
 * the reference count, such as PtrHandle or Util::Ref.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO::IoObject : public virtual Util::Object
{
private:
  IoObject(const IoObject&);
  IoObject& operator=(const IoObject&);

protected:
  /// Default constructor.
  IoObject() throw();

  /// Virtual destructor to ensure correct destruction of subclasses
  virtual ~IoObject() throw();

public:
  /** Subclasses implement this method to save the object's state via
      the generic interface provided by \c IO::Reader. Parsing the
      format of the input is handled by the \c IO::Reader, so
      implementors of \c readFrom() of don't need to deal with
      formatting. */
  virtual void readFrom(IO::Reader& reader) = 0;

  /** Subclasses implement this method to restore the object's state
      via the generic interface provided by \c IO::Writer. Formatting
      the output is handled by the \c IO::Writer, so implementors of
      \c writeTo() of don't need to deal with formatting. */
  virtual void writeTo(IO::Writer& writer) const = 0;

  /** Returns the number of attributes that are written in the
      object's \c writeTo() function. The default implementation
      simply calls writeTo() with a dummy Writer and counts how many
      attributes are written. However, this is somewhat inefficient
      since it defers the counting to runtime when the number may in
      fact be known at compile time. Thus subclasses may wish to
      override this function to return a compile-time constant. */
  virtual unsigned int ioAttribCount() const;

  /** Returns a serialization version id for the class. The default
      implementation returns zero. Classes should override this when
      they make a change that requires a change to their serialization
      protocol. Overriding versions of this function should follow the
      convention that a higher id refers to a later version of the
      class. Implementations of \c IO::Reader and \c IO::Writer will
      provide a way for a class to store and retrieve the
      serialization version of an object. */
  virtual IO::VersionId serialVersionId() const;
};


static const char vcid_io_h[] = "$Header$";
#endif // !IO_H_DEFINED
