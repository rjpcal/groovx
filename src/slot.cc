///////////////////////////////////////////////////////////////////////
//
// slot.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:37:03 1999
// written: Tue Aug 21 14:24:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SLOT_CC_DEFINED
#define SLOT_CC_DEFINED

#include "util/slot.h"

#define NO_TRACE
#include "util/trace.h"

Util::Slot::~Slot() {
DOTRACE("Util::Slot::~Slot");
}

static const char vcid_slot_cc[] = "$Header$";
#endif // !SLOT_CC_DEFINED
