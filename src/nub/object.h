///////////////////////////////////////////////////////////////////////
//
// object.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jun  5 10:23:15 2001
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

#ifndef OBJECT_H_DEFINED
#define OBJECT_H_DEFINED

#include "nub/refcounted.h"
#include "nub/uid.h"

namespace rutz
{
  class fstring;
}

namespace Nub
{
  class Object;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Nub::Object is a base class for objects that are to be exposed to a
 * scripting language. Associates with each object a unique integer
 * identifier. Nub::Object's are reference counted for automatic
 * memory management, and are generally passed around via Nub::Ref's
 * or Nub::SoftRef's, which automatically manage the reference
 * count. Subclass IO::IoObject provides persistence facilities.
 *
 **/
///////////////////////////////////////////////////////////////////////


class Nub::Object : public Nub::RefCounted
{
protected:
  /// Default constructor.
  Object() throw();

  /// Virtual destructor.
  virtual ~Object() throw();

public:
  /** Returns the unique id for this object. The unique id will always
      be strictly positive; zero is always an invalid unique id. */
  Nub::UID id() const throw();

  /// Returns the typename of the full object.
  /** The result is a demangled version of \c typeid(*this).name(), which
      should very closely resemble the way the object was declared in
      source code. */
  rutz::fstring realTypename() const;

  /// Returns the (apparent) typename of the full object.
  /** The default implementation just returns realTypename(). However,
      certain kinds of objects -- e.g., proxy objects -- might usefully
      choose to have objTypename() return something besides the
      realTypename(), in order to masquerade as a different type of
      object.  */
  virtual rutz::fstring objTypename() const;

  /// Returns a short string describing the object by its typename and id.
  rutz::fstring uniqueName() const;

private:
  Nub::UID itsId;
};

static const char vcid_object_h[] = "$Header$";
#endif // !OBJECT_H_DEFINED