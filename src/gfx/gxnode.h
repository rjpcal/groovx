///////////////////////////////////////////////////////////////////////
//
// gxnode.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov  1 18:26:45 2000
// written: Wed Sep 25 18:52:58 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXNODE_H_DEFINED
#define GXNODE_H_DEFINED

#include "io/io.h"

#include "util/signal.h"

namespace Gfx
{
  class Canvas;
  template <class V> class Rect;
  template <class V> class Box;
}

namespace Util
{
  template <class T> class FwdIter;
  template <class T> class Ref;
};

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
  virtual ~GxNode();

  Util::Signal sigNodeChanged;

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
  virtual Util::FwdIter<const Util::Ref<GxNode> > deepChildren();

  /// Get the 2-D z-projection of the result of getBoundingCube().
  Gfx::Rect<double> getBoundingBox(Gfx::Canvas& canvas) const;

  /** Subclasses override this to transform \a bbox according to how that
      subclass will be rendered. */
  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const = 0;

  /// Draw the object on \a canvas.
  virtual void draw(Gfx::Canvas& canvas) const = 0;

  /// Undraw the object from \a canvas by clearing the bounding box.
  void undraw(Gfx::Canvas& canvas) const;
};

static const char vcid_gxnode_h[] = "$Header$";
#endif // !GXNODE_H_DEFINED
