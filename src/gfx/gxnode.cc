///////////////////////////////////////////////////////////////////////
//
// gxnode.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Nov  1 18:27:15 2000
// written: Wed Nov  1 18:32:34 2000
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

static const char vcid_gxnode_cc[] = "$Header$";
#endif // !GXNODE_CC_DEFINED
