///////////////////////////////////////////////////////////////////////
//
// factory.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 22:37:31 1999
// written: Tue Oct 31 13:49:45 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACTORY_CC_DEFINED
#define FACTORY_CC_DEFINED

#include "factory.h"

#include "util/strings.h"

#include "util/hash.h"

#define NO_TRACE
#include "util/trace.h"

namespace {
  const char* bad_create_msg = "unable to create object of type ";
}

FactoryError::FactoryError(const char* str) : ErrorWithMsg(str) {}

FactoryError::~FactoryError() {}

void FactoryError::throwForType(const char* type) {
  FactoryError err(bad_create_msg);
  err.appendMsg(type);
  throw err;
}

struct CreatorMapBase::Impl {
  Impl() : itsMap() {}

  typedef hash_array<fixed_string, void*,
 	 string_hasher<fixed_string> > 
  MapType;

  MapType itsMap;
};

CreatorMapBase::CreatorMapBase() :
  itsImpl(new Impl)
{
DOTRACE("CreatorMapBase::CreatorMapBase");
}

CreatorMapBase::~CreatorMapBase() {
DOTRACE("CreatorMapBase::~CreatorMapBase");
}

void CreatorMapBase::clear() {
DOTRACE("CreatorMapBase::clear");
  for (Impl::MapType::iterator ii = itsImpl->itsMap.begin();
		 ii != itsImpl->itsMap.end();
		 ++ii) {
 	 killPtr(ii->value);
 	 ii->value = 0;
  }

  delete itsImpl;
}

void* CreatorMapBase::getPtrForName(const char* name) const {
DOTRACE("CreatorMapBase::getPtrForName");
  return itsImpl->itsMap[fixed_string(name)];
}

void CreatorMapBase::setPtrForName(const char* name, void* ptr) {
DOTRACE("CreatorMapBase::setPtrForName");
  fixed_string sname(name);
  void*& ptr_slot = itsImpl->itsMap[sname];
  killPtr(ptr_slot);
  ptr_slot = ptr;
}

FactoryBase::~FactoryBase() {
DOTRACE("FactoryBase::~FactoryBase");
}

static const char vcid_factory_cc[] = "$Header$";
#endif // !FACTORY_CC_DEFINED
