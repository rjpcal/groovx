///////////////////////////////////////////////////////////////////////
//
// gxnode.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov  1 18:26:45 2000
// written: Tue Aug 21 15:35:48 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXNODE_H_DEFINED
#define GXNODE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(SIGNAL_H_DEFINED)
#include "util/signal.h"
#endif

namespace Gfx
{
  class Canvas;
  template <class V> class Rect;
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

  /** Subclasses override this to transform \a bbox according to how
      that subclass will be rendered. The default implementation does
      nothing. */
  virtual void getBoundingBox(Gfx::Rect<double>& bbox,
                              Gfx::Canvas& canvas) const;

  /// Draw the object on \a canvas.
  virtual void draw(Gfx::Canvas& canvas) const = 0;

  /// Undraw the object from \a canvas.
  virtual void undraw(Gfx::Canvas& canvas) const = 0;
};

static const char vcid_gxnode_h[] = "$Header$";
#endif // !GXNODE_H_DEFINED
