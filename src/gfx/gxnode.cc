/** @file gfx/gxnode.cc base class for graphic objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov  1 18:27:15 2000
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#include "gxnode.h"

#include "geom/box.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "nub/ref.h"

#include "rutz/iter.h"

#include "rutz/trace.h"

using std::shared_ptr;

class GxNodeIter : public rutz::fwd_iter_ifx<const nub::ref<GxNode> >
{
  nub::ref<GxNode> itsNode;
  bool isItValid;

public:
  GxNodeIter(GxNode* node) :
    itsNode(node), isItValid(true) {}

  typedef const nub::ref<GxNode> ValType;

  virtual rutz::fwd_iter_ifx<ValType>* clone() const
  {
    return new GxNodeIter(*this);
  }

  virtual bool     at_end() const { return !isItValid; }
  virtual ValType&    get() const { return itsNode; }
  virtual void       next()       { isItValid = false; }
};

GxNode::GxNode() : sigNodeChanged()
{
GVX_TRACE("GxNode::GxNode");
}

GxNode::~GxNode() noexcept
{
GVX_TRACE("GxNode::~GxNode");
}

bool GxNode::contains(GxNode* other) const
{
GVX_TRACE("GxNode::contains");
  return (this == other);
}

rutz::fwd_iter<const nub::ref<GxNode> > GxNode::deepChildren()
{
GVX_TRACE("GxNode::deepChildren");

  return rutz::fwd_iter<const nub::ref<GxNode> >
    (std::make_unique<GxNodeIter>(this));
}

geom::rect<double> GxNode::getBoundingBox(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxNode::getBoundingBox");

  Gfx::Bbox bbox(canvas);
  getBoundingCube(bbox);

  return bbox.rect();
}

void GxNode::undraw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxNode::undraw");

  const geom::rect<double> world_box = getBoundingBox(canvas);

  geom::rect<int> screen_box = canvas.screenBoundsFromWorldRect(world_box);

  screen_box.set_width(screen_box.width()+4);
  screen_box.set_height(screen_box.height()+4);

  canvas.clearColorBuffer(screen_box);
}
