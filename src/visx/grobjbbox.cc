///////////////////////////////////////////////////////////////////////
//
// grobjbbox.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 10:45:53 2001
// written: Fri Aug 24 18:36:00 2001
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
#define LOCAL_ASSERT
#include "util/debug.h"

namespace
{
  void renderRect(Gfx::Canvas& canvas, const Gfx::Rect<double>& bounds)
  {
    static unsigned short stipple = 0x0F0F; // 0000111100001111

    static unsigned short mask =    0x3333; // 0011001100110011

    stipple ^= mask;

    mask = ~mask;

    Gfx::Canvas::AttribSaver saver(canvas);

    canvas.setLineWidth(1.0);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, stipple);

    canvas.drawRect(bounds);
  }

  Gfx::Rect<double> addPixelBorder(Gfx::Canvas& canvas,
                                   const Gfx::Rect<double>& raw,
                                   int border_pixels)
  {

    Gfx::Rect<double> result = raw;

    result.widenByFactor(1.0 + border_pixels/100.0);
    result.heightenByFactor(1.0 + border_pixels/100.0);

    return result;
  }
}

Gfx::Rect<double> GrObjBBox::gnodeBoundingBox(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjBBox::gnodeBoundingBox");

  // Add extra pixels if the box itself will be visible.
  int border_pixels = isItVisible ? itsPixelBorder+4 : itsPixelBorder;

  DebugEval(itsPixelBorder); DebugEval(border_pixels);

  return addPixelBorder(canvas,
                        child()->gnodeBoundingBox(canvas),
                        border_pixels);
}

void GrObjBBox::gnodeDraw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjBBox::gnodeDraw");

  if (isItVisible)
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
