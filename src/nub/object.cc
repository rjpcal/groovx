///////////////////////////////////////////////////////////////////////
//
// object.cc
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jun  5 10:26:14 2001
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

#ifndef OBJECT_CC_DEFINED
#define OBJECT_CC_DEFINED

#include "object.h"

#include "util/demangle.h"
#include "util/fstring.h"

#include <typeinfo>

#include "util/trace.h"

namespace
{
  Nub::UID idCounter = 0;
}

Nub::Object::Object() throw() : itsId(++idCounter)
{
DOTRACE("Nub::Object::Object");
}

Nub::Object::~Object() throw()
{
DOTRACE("Nub::Object::~Object");
}

Nub::UID Nub::Object::id() const throw()
{
  return itsId;
}

rutz::fstring Nub::Object::realTypename() const
{
DOTRACE("Nub::Object::realTypename");
  return rutz::demangled_name(typeid(*this));
}

rutz::fstring Nub::Object::objTypename() const
{
DOTRACE("Nub::Object::objTypename");
  return realTypename();
}

rutz::fstring Nub::Object::uniqueName() const
{
DOTRACE("Nub::Object::uniqueName");
  return rutz::fstring(objTypename(), "(", id(), ")");
}

static const char vcid_object_cc[] = "$Header$";
#endif // !OBJECT_CC_DEFINED
