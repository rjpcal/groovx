///////////////////////////////////////////////////////////////////////
//
// volatileobject.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Aug 21 17:17:51 2001
// written: Mon Jan 13 11:08:25 2003
// $Id$
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

bool Util::VolatileObject::isNotShareable() const
{
  return true;
}

static const char vcid_volatileobject_cc[] = "$Header$";
#endif // !VOLATILEOBJECT_CC_DEFINED
