///////////////////////////////////////////////////////////////////////
//
// volatileobject.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Aug 21 17:17:51 2001
// written: Fri Jan 18 16:07:03 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VOLATILEOBJECT_CC_DEFINED
#define VOLATILEOBJECT_CC_DEFINED

#include "util/volatileobject.h"

#include "util/trace.h"
#include "util/debug.h"

Util::VolatileObject::VolatileObject()
{
DOTRACE("Util::VolatileObject::VolatileObject");
  this->incrRefCount();
}

Util::VolatileObject::~VolatileObject()
{
DOTRACE("Util::VolatileObject::~VolatileObject");
}

void Util::VolatileObject::destroy()
{
DOTRACE("Util::VolatileObject::destroy");
  Assert(refCount() == 1);

  this->decrRefCount();
}

bool Util::VolatileObject::isNotShareable() const
{
  return true;
}

static const char vcid_volatileobject_cc[] = "$Header$";
#endif // !VOLATILEOBJECT_CC_DEFINED
