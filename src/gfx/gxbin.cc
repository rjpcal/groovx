///////////////////////////////////////////////////////////////////////
//
// gxbin.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 12:32:03 2002
// written: Wed Nov 13 12:32:26 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXBIN_CC_DEFINED
#define GXBIN_CC_DEFINED

#include "gxbin.h"

GxBin::GxBin() : itsChild() {}

GxBin::GxBin(Util::SoftRef<GxNode> child) : itsChild(child) {}

GxBin::~GxBin() {}

static const char vcid_gxbin_cc[] = "$Header$";
#endif // !GXBIN_CC_DEFINED
