///////////////////////////////////////////////////////////////////////
//
// gxnode.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov  1 18:27:15 2000
// written: Fri Jan 18 16:07:07 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXNODE_CC_DEFINED
#define GXNODE_CC_DEFINED

#include "gfx/gxnode.h"

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

void GxNode::getBoundingBox(Gfx::Rect<double>&, Gfx::Canvas&) const
{
DOTRACE("GxNode::getBoundingBox");
}

static const char vcid_gxnode_cc[] = "$Header$";
#endif // !GXNODE_CC_DEFINED
