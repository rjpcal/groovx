///////////////////////////////////////////////////////////////////////
//
// gxnode.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov  1 18:27:15 2000
// written: Mon Jan 13 11:04:47 2003
// $Id$
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
