///////////////////////////////////////////////////////////////////////
//
// factory.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 22:37:31 1999
// written: Sat Mar  4 00:32:50 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACTORY_CC_DEFINED
#define FACTORY_CC_DEFINED

#include "factory.h"

#include <string>
#include <map>

namespace {
  const string bad_create_msg = "unable to create object of type ";
}

void FactoryError::throwForType(const string& type) {
  throw FactoryError(bad_create_msg+type);
}

struct CreatorMapBase::Impl {
  map<string, void*> itsMap;
};

CreatorMapBase::CreatorMapBase() :
  itsImpl(new Impl)
{}

CreatorMapBase::~CreatorMapBase() {}

void CreatorMapBase::clear() {
  for (map<string, void*>::iterator ii = itsImpl->itsMap.begin();
		 ii != itsImpl->itsMap.end();
		 ++ii) {
	 killPtr(ii->second);
	 ii->second = 0;
  }

  delete itsImpl;
}

void* CreatorMapBase::getPtrForName(const string& name) const {
  return itsImpl->itsMap[name];
}

void CreatorMapBase::setPtrForName(const string& name, void* ptr) {
  killPtr(itsImpl->itsMap[name]);
  itsImpl->itsMap[name] = ptr;
}

static const char vcid_factory_cc[] = "$Header$";
#endif // !FACTORY_CC_DEFINED
