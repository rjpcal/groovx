///////////////////////////////////////////////////////////////////////
//
// gxnode.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov  1 18:27:15 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXNODE_CC_DEFINED
#define GXNODE_CC_DEFINED

#include "gfx/gxnode.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/box.h"

#include "util/iter.h"
#include "util/ref.h"

#include "util/trace.h"

class GxNodeIter : public Util::FwdIterIfx<const Util::Ref<GxNode> >
{
  Util::Ref<GxNode> itsNode;
  bool isItValid;

public:
  GxNodeIter(GxNode* node) :
    itsNode(node), isItValid(true) {}

  typedef const Util::Ref<GxNode> ValType;

  virtual Util::FwdIterIfx<ValType>* clone() const
  {
    return new GxNodeIter(*this);
  }

  virtual bool     atEnd()  const { return !isItValid; }
  virtual ValType&   get()  const { return itsNode; }
  virtual void      next()        { isItValid = false; }
};

GxNode::GxNode() : sigNodeChanged()
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

  return Util::FwdIter<const Util::Ref<GxNode> >
    (shared_ptr<GxNodeIter>(new GxNodeIter(this)));
}

Gfx::Rect<double> GxNode::getBoundingBox(Gfx::Canvas& canvas) const
{
DOTRACE("GxNode::getBoundingBox");

  Gfx::Bbox bbox(canvas);
  getBoundingCube(bbox);

  return bbox.rect();
}

void GxNode::undraw(Gfx::Canvas& canvas) const
{
DOTRACE("GxNode::undraw");

  const Gfx::Rect<double> world_box = getBoundingBox(canvas);

  Gfx::Rect<int> screen_box = canvas.screenFromWorld(world_box);

  screen_box.setWidth(screen_box.width()+4);
  screen_box.setHeight(screen_box.height()+4);

  canvas.clearColorBuffer(screen_box);
}

static const char vcid_gxnode_cc[] = "$Header$";
#endif // !GXNODE_CC_DEFINED
