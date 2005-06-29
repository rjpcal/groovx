///////////////////////////////////////////////////////////////////////
//
// objfactory.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Jun 26 23:40:06 1999
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

#ifndef GROOVX_NUB_OBJFACTORY_H_UTC20050626084019_DEFINED
#define GROOVX_NUB_OBJFACTORY_H_UTC20050626084019_DEFINED

#include "nub/object.h"
#include "nub/ref.h"

#include "rutz/factory.h"

namespace Nub
{
  class ObjFactory;
}

/// Singleton wrapper for Factor<IO>.
class Nub::ObjFactory
  :
  public rutz::factory<Nub::SoftRef<Nub::Object> >
{
protected:
  /// Default constructor.
  ObjFactory();

  /// Virtual destructor.
  virtual ~ObjFactory() throw();

public:
  /// Return the singleton instance.
  static ObjFactory& theOne();
};

static const char vcid_groovx_nub_objfactory_h_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_OBJFACTORY_H_UTC20050626084019_DEFINED
