///////////////////////////////////////////////////////////////////////
//
// gnode.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 10 13:36:59 2001
// written: Fri Aug 10 16:30:56 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GNODE_H_DEFINED
#define GNODE_H_DEFINED

#include "util/pointers.h"

namespace Gfx
{
  class Canvas;
}

template <class V> class Rect;

class Gnode {
private:
  shared_ptr<Gnode> itsChild;

protected:
  Gnode* child() const { return itsChild.get(); }

public:
  Gnode() : itsChild() {}

  Gnode(shared_ptr<Gnode> child) : itsChild(child) {}

  virtual ~Gnode();

  void setChild(shared_ptr<Gnode> child) { itsChild = child; }

  virtual void gnodeDraw(Gfx::Canvas& canvas) const = 0;

  virtual void gnodeUndraw(Gfx::Canvas& canvas) const = 0;

  virtual Rect<double> gnodeBoundingBox(Gfx::Canvas& canvas) const = 0;
};

static const char vcid_gnode_h[] = "$Header$";
#endif // !GNODE_H_DEFINED
