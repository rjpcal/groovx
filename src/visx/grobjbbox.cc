///////////////////////////////////////////////////////////////////////
//
// grobjbbox.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 10:45:53 2001
// written: Tue Aug 14 11:52:22 2001
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
}

Gfx::Rect<double> GrObjBBox::gnodeBoundingBox(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjBBox::withBorder");

  const Gfx::Rect<double> native = child()->gnodeBoundingBox(canvas);

  // Do the object's internal scaling and alignment, and find the
  // bounding box in screen coordinates

  Gfx::Rect<int> screen_pos;

  screen_pos = canvas.screenFromWorld(native);

  // Add a pixel border around the edges of the image...
  int bp = pixelBorder();

  screen_pos.widenByStep(bp);
  screen_pos.heightenByStep(bp);

  // ... and project back to world coordinates
  return canvas.worldFromScreen(screen_pos);
}

void GrObjBBox::gnodeDraw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjBBox::gnodeDraw");

  if (itsIsVisible)
    {
      Gfx::Rect<double> bounds = gnodeBoundingBox(canvas);

      renderRect(canvas, bounds);
    }

  child()->gnodeDraw(canvas);
}

void GrObjBBox::gnodeUndraw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjBBox::gnodeUndraw");

  if (itsIsVisible)
    {
      Gfx::Rect<double> bounds = gnodeBoundingBox(canvas);

      renderRect(canvas, bounds);
    }

  child()->gnodeUndraw(canvas);
}

static const char vcid_grobjbbox_cc[] = "$Header$";
#endif // !GROBJBBOX_CC_DEFINED
