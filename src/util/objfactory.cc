///////////////////////////////////////////////////////////////////////
//
// iofactory.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun 30 15:01:02 1999
// written: Thu Mar 30 12:09:09 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOFACTORY_CC_DEFINED
#define IOFACTORY_CC_DEFINED

#include "io/iofactory.h"

namespace {
  IO::IoFactory* instance = 0;
}

IO::IoFactory::IoFactory() :
  Factory<IO::IoObject>() {}

IO::IoFactory& IO::IoFactory::theOne() {
  if (instance == 0) {
	 instance = new IoFactory;
  }
  return *instance;
}

static const char vcid_iofactory_cc[] = "$Header$";
#endif // !IOFACTORY_CC_DEFINED
