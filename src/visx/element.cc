///////////////////////////////////////////////////////////////////////
//
// element.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  4 15:40:25 2002
// written: Thu Dec  5 14:35:53 2002
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
