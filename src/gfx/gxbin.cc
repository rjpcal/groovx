///////////////////////////////////////////////////////////////////////
//
// gxbin.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 12:32:03 2002
// written: Thu Nov 14 17:54:40 2002
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

void GxBin::setChild(const Util::Ref<GxNode>& child)
{
  itsChild->sigNodeChanged.disconnect(this->sigNodeChanged.slot());
  itsChild = child;
  itsChild->sigNodeChanged.connect(this->sigNodeChanged.slot());
  this->sigNodeChanged.emit();
}

static const char vcid_gxbin_cc[] = "$Header$";
#endif // !GXBIN_CC_DEFINED
