///////////////////////////////////////////////////////////////////////
//
// element.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Dec  4 15:40:25 2002
// written: Wed Mar 19 12:45:33 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ELEMENT_CC_DEFINED
#define ELEMENT_CC_DEFINED

#include "visx/element.h"

Element::~Element() {}

void Element::vxEndTrialHook() { /* no-op */ }

static const char vcid_element_cc[] = "$Header$";
#endif // !ELEMENT_CC_DEFINED
