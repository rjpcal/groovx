///////////////////////////////////////////////////////////////////////
//
// observer.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 25 18:37:03 1999
// written: Tue Oct 12 15:13:13 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVER_CC_DEFINED
#define OBSERVER_CC_DEFINED

#include "observer.h"

#define NO_TRACE
#include "trace.h"

Observer::~Observer() {
DOTRACE("Observer::~Observer");
}

static const char vcid_observer_cc[] = "$Header$";
#endif // !OBSERVER_CC_DEFINED
