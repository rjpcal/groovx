///////////////////////////////////////////////////////////////////////
//
// gxnode.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov  1 18:27:15 2000
// written: Fri Aug 17 10:26:39 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXNODE_CC_DEFINED
#define GXNODE_CC_DEFINED

#include "gfx/gxnode.h"

#include "gfx/gxtraversal.h"

#include "util/ref.h"

#include "util/trace.h"

GxNode::GxNode()
{
DOTRACE("GxNode::GxNode");
}

GxNode::~GxNode()
{
DOTRACE("GxNode::~GxNode");
}

int GxNode::category() const
{
DOTRACE("GxNode::category");
  return -1;
}

bool GxNode::contains(GxNode* other) const
{
DOTRACE("GxNode::contains");
  return (this == other);
}

Util::FwdIter<const Util::Ref<GxNode> > GxNode::deepChildren()
{
DOTRACE("GxNode::deepChildren");

  return GxTraversal(Util::Ref<GxNode>(this));
}

void GxNode::getBoundingBox(Gfx::Rect<double>&, Gfx::Canvas&) const
{
DOTRACE("GxNode::getBoundingBox");
}

static const char vcid_gxnode_cc[] = "$Header$";
#endif // !GXNODE_CC_DEFINED
