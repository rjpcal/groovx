///////////////////////////////////////////////////////////////////////
//
// refcounted.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Oct 22 14:40:28 2000
// written: Tue Jun  5 10:24:16 2001
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

void* Util::RefCounted::operator new(size_t bytes) {
DOTRACE("Util::RefCounted::operator new");
  return ::operator new(bytes);
}

void Util::RefCounted::operator delete(void* space, size_t bytes) {
DOTRACE("Util::RefCounted::operator delete");
  ::operator delete(space);
}

Util::RefCounted::RefCounted() :
  itsRefCount(0)
{
DOTRACE("Util::RefCounted::RefCounted");
 DebugEval((void*)this); 
}

Util::RefCounted::~RefCounted()
{
DOTRACE("Util::RefCounted::~RefCounted");
  DebugEval((void*)this); DebugEvalNL(itsRefCount);
  Assert(itsRefCount <= 0);
}

void Util::RefCounted::incrRefCount() const {
DOTRACE("Util::RefCounted::incrRefCount");
  DebugEval((void*)this);
  ++itsRefCount;
}

void Util::RefCounted::decrRefCount() const {
DOTRACE("Util::RefCounted::decrRefCount");
  DebugEval((void*)this); 

  --itsRefCount;
  if (itsRefCount <= 0)
	 delete this;
}

bool Util::RefCounted::isShared() const {
DOTRACE("Util::RefCounted::isShared");
  return (itsRefCount > 1);
}

bool Util::RefCounted::isUnshared() const {
DOTRACE("Util::RefCounted::isUnshared");
  return !isShared();
}

int Util::RefCounted::refCount() const {
DOTRACE("Util::RefCounted::refCount");
  return itsRefCount;
}

static const char vcid_refcounted_cc[] = "$Header$";
#endif // !REFCOUNTED_CC_DEFINED
