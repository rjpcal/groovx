///////////////////////////////////////////////////////////////////////
//
// iofactory.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun 30 15:01:02 1999
// written: Wed Mar 29 23:56:37 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOFACTORY_CC_DEFINED
#define IOFACTORY_CC_DEFINED

#include "io/iofactory.h"

namespace {
  IoFactory* instance = 0;
}

IoFactory::IoFactory() :
  Factory<IO>() {}

IoFactory& IoFactory::theOne() {
  if (instance == 0) {
	 instance = new IoFactory;
  }
  return *instance;
}

static const char vcid_iofactory_cc[] = "$Header$";
#endif // !IOFACTORY_CC_DEFINED
