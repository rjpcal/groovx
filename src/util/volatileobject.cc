///////////////////////////////////////////////////////////////////////
//
// volatileobject.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Aug 21 17:17:51 2001
// written: Tue Sep  4 07:32:09 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VOLATILEOBJECT_CC_DEFINED
#define VOLATILEOBJECT_CC_DEFINED

#include "util/volatileobject.h"

#include "util/trace.h"
#define LOCAL_ASSERT
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
