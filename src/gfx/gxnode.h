///////////////////////////////////////////////////////////////////////
//
// gxnode.h
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov  1 18:26:45 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef GXNODE_H_DEFINED
#define GXNODE_H_DEFINED

#include "io/io.h"

#include "util/signal.h"

namespace rutz
{
  template <class T> class fwd_iter;
}

namespace Gfx
{
  class Bbox;
  class Canvas;
  template <class V> class Rect;
  template <class V> class Box;
}

namespace Nub
{
  template <class T> class Ref;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * GxNode is the base class for all graphic objects.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GxNode : public IO::IoObject
{
private:
  GxNode(const GxNode&);
  GxNode& operator=(const GxNode&);

protected:
  /// Default constructor.
  GxNode();

public:
  /// Virtual destructor.
  virtual ~GxNode() throw();

  /// Signal that will be triggered whenever the node changes state.
  Nub::Signal0 sigNodeChanged;

  /// Get the value of the user-defined category. Default returns -1.
  virtual int category() const;

  /** Returns true if \a other is contained within this node in the
      scene graph. The default implementation (for leaf nodes) returns
      true only if this == other. For composite nodes, the function
      will check recursively is \a other is contained within the
      composite structure. This function is used to avoid generated
      cycles in the scene graph. */
  virtual bool contains(GxNode* other) const;

  /** Returns an iterator to all the children recursively contained in
      this object. */
  virtual rutz::fwd_iter<const Nub::Ref<GxNode> > deepChildren();

  /// Get the 2-D z-projection of the result of getBoundingCube().
  Gfx::Rect<double> getBoundingBox(Gfx::Canvas& canvas) const;

  /** Subclasses override this to transform the bounding box according to
      how that subclass is rendered. */
  virtual void getBoundingCube(Gfx::Bbox& bbox) const = 0;

  /// Draw the object on \a canvas.
  virtual void draw(Gfx::Canvas& canvas) const = 0;

  /// Undraw the object from \a canvas by clearing the bounding box.
  void undraw(Gfx::Canvas& canvas) const;
};

static const char vcid_gxnode_h[] = "$Header$";
#endif // !GXNODE_H_DEFINED
