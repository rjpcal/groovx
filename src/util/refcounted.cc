///////////////////////////////////////////////////////////////////////
//
// refcounted.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Oct 22 14:40:28 2000
// written: Sun Oct 22 15:01:54 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef REFCOUNTED_CC_DEFINED
#define REFCOUNTED_CC_DEFINED

#include "util/refcounted.h"

#include "util/error.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// RefCounted member definitions
//
///////////////////////////////////////////////////////////////////////

RefCounted::RefCounted() :
  itsRefCount(0)
{
DOTRACE("RefCounted::RefCounted");
}

RefCounted::~RefCounted()
{
DOTRACE("RefCounted::~RefCounted");
}

void RefCounted::incrRefCount() {
DOTRACE("RefCounted::incrRefCount");
  ++itsRefCount;
}

void RefCounted::decrRefCount() {
DOTRACE("RefCounted::decrRefCount");
  --itsRefCount;
  if (itsRefCount <= 0)
	 delete this;
}

bool RefCounted::isShared() const {
DOTRACE("RefCounted::isShared");
  return (itsRefCount > 1);
}

bool RefCounted::isUnshared() const {
DOTRACE("RefCounted::isUnshared");
  return (itsRefCount <= 1);
}

int RefCounted::refCount() const {
DOTRACE("RefCounted::refCount");
  return itsRefCount;
}

bool RefCounted::isValid() const {
DOTRACE("RefCounted::isValid");
  return true;
}

static const char vcid_refcounted_cc[] = "$Header$";
#endif // !REFCOUNTED_CC_DEFINED
