///////////////////////////////////////////////////////////////////////
//
// gnode.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 10 13:36:59 2001
// written: Wed Nov 13 10:49:56 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GNODE_H_DEFINED
#define GNODE_H_DEFINED

#include "gfx/gxnode.h"

#include "gx/box.h"
#include "gx/rect.h"

#include "util/ref.h"

namespace Gfx
{
  class Canvas;
  template <class V> class Rect;
}

class Gnode : public GxNode
{
private:
  Util::SoftRef<Gnode> itsChild;

protected:
  Gnode* child() const { return itsChild.get(); }

public:
  Gnode() : itsChild() {}

  Gnode(Util::SoftRef<Gnode> child) : itsChild(child) {}

  virtual ~Gnode();

  void setChild(Util::SoftRef<Gnode> child) { itsChild = child; }

  virtual void gnodeBoundingBox(Gfx::Box<double>& cube,
                                Gfx::Canvas& canvas) const = 0;

  virtual void readFrom(IO::Reader* /*reader*/) {};
  virtual void writeTo(IO::Writer* /*writer*/) const {};

  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const
  {
    gnodeBoundingBox(cube, canvas);
//     cube.unionize(gnodeBoundingBox(canvas));
  }

  virtual void draw(Gfx::Canvas& canvas) const = 0;
};

static const char vcid_gnode_h[] = "$Header$";
#endif // !GNODE_H_DEFINED
