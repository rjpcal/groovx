///////////////////////////////////////////////////////////////////////
//
// grobjbbox.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 10:45:53 2001
// written: Thu Aug 16 11:32:31 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJBBOX_CC_DEFINED
#define GROBJBBOX_CC_DEFINED

#include "grobjbbox.h"

#include "gfx/canvas.h"
#include "gfx/rect.h"

#include <GL/gl.h>

#include "util/trace.h"

namespace
{
  void renderRect(Gfx::Canvas& canvas, const Gfx::Rect<double>& bounds)
  {
    glPushAttrib(GL_LINE_BIT);
    {
      glLineWidth(1.0);
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(1, 0x0F0F);

      canvas.drawRect(bounds);
    }
    glPopAttrib();
  }

  Gfx::Rect<double> addPixelBorder(Gfx::Canvas& canvas,
											  const Gfx::Rect<double>& raw,
											  int border_pixels)
  {
    // Find the bounding box in screen coordinates
    Gfx::Rect<int> screen_pos = canvas.screenFromWorld(raw);

    // Add the pixel border around the edges...
    screen_pos.widenByStep(border_pixels);
    screen_pos.heightenByStep(border_pixels);

    // ... and project back to world coordinates
    return canvas.worldFromScreen(screen_pos);
  }
}

Gfx::Rect<double> GrObjBBox::gnodeBoundingBox(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjBBox::withBorder");

  // Add extra pixels if the box itself will be visible.
  int border_pixels = itsIsVisible ? itsPixelBorder+2 : itsPixelBorder;

  return addPixelBorder(canvas,
								child()->gnodeBoundingBox(canvas),
								border_pixels);
}

void GrObjBBox::gnodeDraw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjBBox::gnodeDraw");

  if (itsIsVisible)
    {
      Gfx::Rect<double> bounds = 
        addPixelBorder(canvas,
							  child()->gnodeBoundingBox(canvas),
							  itsPixelBorder);

      renderRect(canvas, bounds);
    }

  child()->gnodeDraw(canvas);
}

static const char vcid_grobjbbox_cc[] = "$Header$";
#endif // !GROBJBBOX_CC_DEFINED
