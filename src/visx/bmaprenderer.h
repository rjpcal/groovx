///////////////////////////////////////////////////////////////////////
//
// bmaprenderer.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 16:51:34 1999
// written: Fri Aug 10 10:50:05 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPRENDERER_H_DEFINED
#define BMAPRENDERER_H_DEFINED

namespace Gfx
{
  class Canvas;
}

class BmapData;

template <class V> class Point;
template <class V> class Rect;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c BmapRenderer defines an abstract interface for rendering raw
 * bitmap data to the screen.
 *
 **/
///////////////////////////////////////////////////////////////////////

class BmapRenderer {
public:
  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~BmapRenderer();

  /** This is the operation that subclasses must implement to do the
      actual rendering of the bitmap data. All of the information
      needed to do the rendering is passed as arguments to the
      function. */
  virtual void doRender(Gfx::Canvas& canvas,
                        const BmapData& data,
                        const Point<double>& world_pos,
                        const Point<double>& zoom) const = 0;

  /** This operation should be called by clients after every change to
      the bitmap data. The default implementation provided by Bitmap
      is a no-op, but subclasses may override if they need to update
      data structures when the bitmap data change. */
  virtual void notifyBytesChanged() const;
};

static const char vcid_bmaprenderer_h[] = "$Header$";
#endif // !BMAPRENDERER_H_DEFINED
