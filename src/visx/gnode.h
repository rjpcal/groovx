///////////////////////////////////////////////////////////////////////
//
// gnode.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 10 13:36:59 2001
// written: Wed Nov 13 12:29:16 2002
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
  Util::SoftRef<GxNode> itsChild;

public:
  Gnode() : itsChild() {}

  Gnode(Util::SoftRef<GxNode> child) : itsChild(child) {}

  virtual ~Gnode();

  const Util::SoftRef<GxNode>& child() const { return itsChild; }

  void setChild(Util::SoftRef<GxNode> child) { itsChild = child; }
};

static const char vcid_gnode_h[] = "$Header$";
#endif // !GNODE_H_DEFINED
