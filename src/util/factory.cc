///////////////////////////////////////////////////////////////////////
//
// factory.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 22:37:31 1999
// written: Wed Mar  8 11:35:45 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACTORY_CC_DEFINED
#define FACTORY_CC_DEFINED

#include "factory.h"

#include <string>
#include <map>

namespace {
  const char* bad_create_msg = "unable to create object of type ";
}

void FactoryError::throwForType(const char* type) {
  FactoryError err(bad_create_msg);
  err.appendMsg(type);
  throw err;
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

void* CreatorMapBase::getPtrForName(const char* name) const {
  return itsImpl->itsMap[string(name)];
}

void CreatorMapBase::setPtrForName(const char* name, void* ptr) {
  string sname(name);
  killPtr(itsImpl->itsMap[sname]);
  itsImpl->itsMap[sname] = ptr;
}

static const char vcid_factory_cc[] = "$Header$";
#endif // !FACTORY_CC_DEFINED
