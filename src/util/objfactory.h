///////////////////////////////////////////////////////////////////////
//
// objfactory.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Jun 26 23:40:06 1999
// written: Wed Mar 19 17:58:53 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJFACTORY_H_DEFINED
#define OBJFACTORY_H_DEFINED

#include "util/factory.h"
#include "util/object.h"
#include "util/ref.h"

namespace Util
{
  class ObjFactory;
}

/// Singleton wrapper for Factor<IO>.
class Util::ObjFactory : public Factory<Util::SoftRef<Util::Object> >
{
protected:
  /// Default constructor.
  ObjFactory();

  /// Virtual destructor.
  virtual ~ObjFactory();

public:
  /// Return the singleton instance.
  static ObjFactory& theOne();
};

static const char vcid_objfactory_h[] = "$Header$";
#endif // !OBJFACTORY_H_DEFINED
