///////////////////////////////////////////////////////////////////////
//
// gxnode.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Nov  1 18:27:15 2000
// written: Thu Nov  2 13:50:52 2000
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

int GxNode::getCategory() const {
DOTRACE("GxNode::getCategory");
  return -1;
}

bool GxNode::contains(GxNode* other) const {
DOTRACE("GxNode::contains");
  return (this == other);
}

static const char vcid_gxnode_cc[] = "$Header$";
#endif // !GXNODE_CC_DEFINED
