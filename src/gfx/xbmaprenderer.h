///////////////////////////////////////////////////////////////////////
//
// xbmaprenderer.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Dec  1 17:19:23 1999
// written: Wed Mar 19 12:45:56 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBMAPRENDERER_H_DEFINED
#define XBMAPRENDERER_H_DEFINED

#include <X11/Xlib.h>

namespace Gfx
{
  class BmapData;
  class Canvas;
  template <class V> class Vec2;
}

/// XBmapRenderer is "in between jobs"... needs to be merged into GLCanvas.
namespace XBmapRenderer
{
   void doRender(Display* display,
                 Window win,
                 int screen,
                 Gfx::Canvas& canvas,
                 const Gfx::BmapData& data,
                 const Gfx::Vec2<double>& world_pos,
                 const Gfx::Vec2<double>& zoom);
};

static const char vcid_xbmaprenderer_h[] = "$Header$";
#endif // !XBMAPRENDERER_H_DEFINED
