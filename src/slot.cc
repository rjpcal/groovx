///////////////////////////////////////////////////////////////////////
//
// observer.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 25 18:37:03 1999
// written: Thu Mar 30 00:15:08 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVER_CC_DEFINED
#define OBSERVER_CC_DEFINED

#include "util/observer.h"

#define NO_TRACE
#include "util/trace.h"

Observer::~Observer() {
DOTRACE("Observer::~Observer");
}

static const char vcid_observer_cc[] = "$Header$";
#endif // !OBSERVER_CC_DEFINED
