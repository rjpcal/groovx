///////////////////////////////////////////////////////////////////////
//
// xbmaprenderer.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 17:19:23 1999
// written: Wed Nov 20 17:19:44 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBMAPRENDERER_H_DEFINED
#define XBMAPRENDERER_H_DEFINED

struct _XImage;
typedef _XImage XImage;

struct Tk_Window_;
typedef Tk_Window_* Tk_Window;

namespace Gfx
{
  class BmapData;
  class Canvas;
  template <class V> class Rect;
  template <class V> class Vec2;
}

/// XBmapRenderer is "in between jobs"... needs to be merged into GLCanvas.
class XBmapRenderer
{
public:
  /** Initializes some X11 structures. Must be called before any \c
      XBmapRenderer objects are rendered. */
  static void initClass(Tk_Window win);

  /// Default constructor.
  XBmapRenderer();

  /// Virtual destructor.
  virtual ~XBmapRenderer();

  virtual void doRender(Gfx::Canvas& canvas,
                        const Gfx::BmapData& data,
                        const Gfx::Vec2<double>& world_pos,
                        const Gfx::Vec2<double>& zoom) const;

private:
  void update(const Gfx::BmapData& data) const;

  XBmapRenderer(const XBmapRenderer&);
  XBmapRenderer& operator=(const XBmapRenderer&);

  mutable XImage* itsImage;
};

static const char vcid_xbmaprenderer_h[] = "$Header$";
#endif // !XBMAPRENDERER_H_DEFINED
