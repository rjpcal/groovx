///////////////////////////////////////////////////////////////////////
//
// gxbin.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 12:32:03 2002
// written: Thu Nov 14 17:04:31 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXBIN_CC_DEFINED
#define GXBIN_CC_DEFINED

#include "gxbin.h"

#include "gfx/gxemptynode.h"

GxBin::GxBin() : itsChild(GxEmptyNode::make()) {}

GxBin::GxBin(Util::Ref<GxNode> child) : itsChild(child) {}

GxBin::~GxBin() {}

static const char vcid_gxbin_cc[] = "$Header$";
#endif // !GXBIN_CC_DEFINED
