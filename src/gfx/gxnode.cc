///////////////////////////////////////////////////////////////////////
//
// gxnode.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov  1 18:27:15 2000
// written: Thu May 10 12:04:44 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXNODE_CC_DEFINED
#define GXNODE_CC_DEFINED

#include "gx/gxnode.h"

#include "util/trace.h"

GxNode::GxNode() {
DOTRACE("GxNode::GxNode");
}

GxNode::~GxNode() {
DOTRACE("GxNode::~GxNode");
}

int GxNode::category() const {
DOTRACE("GxNode::category");
  return -1;
}

bool GxNode::contains(GxNode* other) const {
DOTRACE("GxNode::contains");
  return (this == other);
}

static const char vcid_gxnode_cc[] = "$Header$";
#endif // !GXNODE_CC_DEFINED
