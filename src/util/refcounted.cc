///////////////////////////////////////////////////////////////////////
//
// refcounted.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Oct 22 14:40:28 2000
// written: Sat Jun  2 15:50:12 2001
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

void* RefCounted::operator new(size_t bytes) {
DOTRACE("RefCounted::operator new");
  return ::operator new(bytes);
}

void RefCounted::operator delete(void* space, size_t bytes) {
DOTRACE("RefCounted::operator delete");
  ::operator delete(space);
}

RefCounted::RefCounted() :
  itsRefCount(0),
  itsRefCount2(0)
{
DOTRACE("RefCounted::RefCounted");
 DebugEval((void*)this); 
}

RefCounted::~RefCounted()
{
DOTRACE("RefCounted::~RefCounted");
 DebugEval((void*)this); 
 DebugEval(itsRefCount); DebugEvalNL(itsRefCount2); 
  Assert(itsRefCount <= 0 && itsRefCount2 <= 0);
}

void RefCounted::incrRefCount() const {
DOTRACE("RefCounted::incrRefCount");
 DebugEval((void*)this); 
  ++itsRefCount;
}

void RefCounted::decrRefCount() const {
DOTRACE("RefCounted::decrRefCount");
 DebugEval((void*)this); 
  --itsRefCount;
  if (itsRefCount <= 0 && itsRefCount2 <= 0)
	 delete this;
}

void RefCounted::incrRefCount2() const {
DOTRACE("RefCounted::incrRefCount2");
  ++itsRefCount2;
}

void RefCounted::decrRefCount2() const {
DOTRACE("RefCounted::decrRefCount2");
  --itsRefCount2;
  if (itsRefCount <= 0 && itsRefCount2 <= 0)
	 delete this;
}

bool RefCounted::isShared() const {
DOTRACE("RefCounted::isShared");
  return (itsRefCount > 1 ||
			 itsRefCount2 > 1 ||
			 (itsRefCount + itsRefCount2) > 1
			 );
}

bool RefCounted::isUnshared() const {
DOTRACE("RefCounted::isUnshared");
  return !isShared();
}

int RefCounted::refCount() const {
DOTRACE("RefCounted::refCount");
  return itsRefCount + itsRefCount2;
}

int RefCounted::refCount1() const {
DOTRACE("RefCounted::refCount1");
  return itsRefCount;
}

int RefCounted::refCount2() const {
DOTRACE("RefCounted::refCount2");
  return itsRefCount2;
}

static const char vcid_refcounted_cc[] = "$Header$";
#endif // !REFCOUNTED_CC_DEFINED
