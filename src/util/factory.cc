///////////////////////////////////////////////////////////////////////
//
// factory.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 22:37:31 1999
// written: Tue Oct 31 12:17:34 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACTORY_CC_DEFINED
#define FACTORY_CC_DEFINED

#include "factory.h"

#include "util/strings.h"

#include <map>

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

  std::map<fixed_string, void*> itsMap;
};

CreatorMapBase::CreatorMapBase() :
  itsImpl(new Impl)
{}

CreatorMapBase::~CreatorMapBase() {}

void CreatorMapBase::clear() {
  for (std::map<fixed_string, void*>::iterator ii = itsImpl->itsMap.begin();
		 ii != itsImpl->itsMap.end();
		 ++ii) {
	 killPtr(ii->second);
	 ii->second = 0;
  }

  delete itsImpl;
}

void* CreatorMapBase::getPtrForName(const char* name) const {
  return itsImpl->itsMap[fixed_string(name)];
}

void CreatorMapBase::setPtrForName(const char* name, void* ptr) {
  fixed_string sname(name);
  killPtr(itsImpl->itsMap[sname]);
  itsImpl->itsMap[sname] = ptr;
}

FactoryBase::~FactoryBase() {}

static const char vcid_factory_cc[] = "$Header$";
#endif // !FACTORY_CC_DEFINED
