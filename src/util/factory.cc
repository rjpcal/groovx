///////////////////////////////////////////////////////////////////////
//
// factory.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 22:37:31 1999
// written: Sat Nov 20 22:45:30 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACTORY_CC_DEFINED
#define FACTORY_CC_DEFINED

#include "factory.h"

#include <map>

struct CreatorMapBase::Impl {
  map<string, void*> itsMap;
};

CreatorMapBase::CreatorMapBase() :
  itsImpl(new Impl)
{}

CreatorMapBase::~CreatorMapBase() {
  for (map<string, void*>::iterator ii = itsImpl->itsMap.begin();
		 ii != itsImpl->itsMap.end();
		 ++ii) {
	 destroy(ii->second);
	 ii->second = 0;
  }

  delete itsImpl;
}

void* CreatorMapBase::getPtrForName(const string& name) const {
  return itsImpl->itsMap[name];
}

void CreatorMapBase::setPtrForName(const string& name, void* ptr) {
  destroy(itsImpl->itsMap[name]);
  itsImpl->itsMap[name] = ptr;
}

static const char vcid_factory_cc[] = "$Header$";
#endif // !FACTORY_CC_DEFINED
