///////////////////////////////////////////////////////////////////////
//
// iofactory.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 30 15:01:02 1999
// written: Fri May 18 16:26:06 2001
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
  Factory<IdItem<IO::IoObject> >() {}

IO::IoFactory::~IoFactory() {}

IO::IoFactory& IO::IoFactory::theOne() {
  if (instance == 0) {
	 instance = new IoFactory;
  }
  return *instance;
}

static const char vcid_iofactory_cc[] = "$Header$";
#endif // !IOFACTORY_CC_DEFINED
