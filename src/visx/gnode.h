///////////////////////////////////////////////////////////////////////
//
// gnode.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 10 13:36:59 2001
// written: Fri Aug 24 16:31:12 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GNODE_H_DEFINED
#define GNODE_H_DEFINED

#include "util/ref.h"

namespace Gfx
{
  class Canvas;
  template <class V> class Rect;
}

class Gnode : public Util::Object {
private:
  Util::SoftRef<Gnode> itsChild;

protected:
  Gnode* child() const { return itsChild.get(); }

public:
  Gnode() : itsChild() {}

  Gnode(Util::SoftRef<Gnode> child) : itsChild(child) {}

  virtual ~Gnode();

  void setChild(Util::SoftRef<Gnode> child) { itsChild = child; }

  virtual void gnodeDraw(Gfx::Canvas& canvas) const = 0;

  virtual Gfx::Rect<double> gnodeBoundingBox(Gfx::Canvas& canvas) const = 0;
};

static const char vcid_gnode_h[] = "$Header$";
#endif // !GNODE_H_DEFINED
