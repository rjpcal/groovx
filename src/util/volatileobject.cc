///////////////////////////////////////////////////////////////////////
//
// volatileobject.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Aug 21 17:17:51 2001
// commit: $Id$
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

#ifndef VOLATILEOBJECT_CC_DEFINED
#define VOLATILEOBJECT_CC_DEFINED

#include "util/volatileobject.h"

#include "util/trace.h"
#include "util/debug.h"

Util::VolatileObject::VolatileObject() :
  isItDestroyed(false)
{
DOTRACE("Util::VolatileObject::VolatileObject");
  this->markAsVolatile();
  this->incrRefCount();
}

Util::VolatileObject::~VolatileObject()
{
DOTRACE("Util::VolatileObject::~VolatileObject");

  // If this destructor was not called via destroy(), then it means we
  // probably have a stack object here. In order to keep weak references
  // doing the right thing, we need to make sure that we decrement the ref
  // count here.
  if (!isItDestroyed)
    {
      Assert(refCount() == 1);
      this->decrRefCountNoDelete();
    }
}

void Util::VolatileObject::destroy()
{
DOTRACE("Util::VolatileObject::destroy");
  Assert(refCount() == 1);

  isItDestroyed = true;

  this->decrRefCount();
}

static const char vcid_volatileobject_cc[] = "$Header$";
#endif // !VOLATILEOBJECT_CC_DEFINED
