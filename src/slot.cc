///////////////////////////////////////////////////////////////////////
//
// observer.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 25 18:37:03 1999
// written: Wed Mar 15 10:17:27 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVER_CC_DEFINED
#define OBSERVER_CC_DEFINED

#include "observer.h"

#define NO_TRACE
#include "util/trace.h"

Observer::~Observer() {
DOTRACE("Observer::~Observer");
}

static const char vcid_observer_cc[] = "$Header$";
#endif // !OBSERVER_CC_DEFINED
