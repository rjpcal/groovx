///////////////////////////////////////////////////////////////////////
//
// bmaprenderer.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 16:51:34 1999
// written: Fri Jan 18 16:06:53 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPRENDERER_H_DEFINED
#define BMAPRENDERER_H_DEFINED

namespace Gfx
{
  class BmapData;
  class Canvas;
  template <class V> class Rect;
  template <class V> class Vec2;
}


///////////////////////////////////////////////////////////////////////
/**
 *
 * \c BmapRenderer defines an abstract interface for rendering raw
 * bitmap data to the screen.
 *
 **/
///////////////////////////////////////////////////////////////////////

class BmapRenderer
{
public:
  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~BmapRenderer();

  /** This is the operation that subclasses must implement to do the
      actual rendering of the bitmap data. All of the information
      needed to do the rendering is passed as arguments to the
      function. */
  virtual void doRender(Gfx::Canvas& canvas,
                        const Gfx::BmapData& data,
                        const Gfx::Vec2<double>& world_pos,
                        const Gfx::Vec2<double>& zoom) const = 0;
};

static const char vcid_bmaprenderer_h[] = "$Header$";
#endif // !BMAPRENDERER_H_DEFINED
