///////////////////////////////////////////////////////////////////////
//
// observer.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:37:03 1999
// written: Thu May 10 12:04:42 2001
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
