///////////////////////////////////////////////////////////////////////
//
// gxemptynode.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 14 16:59:58 2002
// written: Thu Nov 14 17:07:23 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXEMPTYNODE_CC_DEFINED
#define GXEMPTYNODE_CC_DEFINED

#include "gxemptynode.h"

GxEmptyNode::GxEmptyNode() {}

GxEmptyNode::~GxEmptyNode() {}

void GxEmptyNode::readFrom(IO::Reader*) {}
void GxEmptyNode::writeTo(IO::Writer*) const {}

void GxEmptyNode::getBoundingCube(Gfx::Box<double>&, Gfx::Canvas&) const {}

void GxEmptyNode::draw(Gfx::Canvas&) const {}

GxEmptyNode* GxEmptyNode::make()
{
  static GxEmptyNode* p = 0;

  if (p == 0)
    {
      p = new GxEmptyNode;
      p->incrRefCount();
    }

  return p;
}

static const char vcid_gxemptynode_cc[] = "$Header$";
#endif // !GXEMPTYNODE_CC_DEFINED
