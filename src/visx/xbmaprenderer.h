///////////////////////////////////////////////////////////////////////
//
// xbmaprenderer.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 17:19:23 1999
// written: Thu Nov 21 09:05:22 2002
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
