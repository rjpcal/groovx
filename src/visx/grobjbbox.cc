///////////////////////////////////////////////////////////////////////
//
// grobjbbox.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 10:45:53 2001
// written: Fri Aug 10 16:36:11 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJBBOX_CC_DEFINED
#define GROBJBBOX_CC_DEFINED

#include "grobjbbox.h"

#include "grobjimpl.h"
#include "rect.h"

#include "gfx/canvas.h"

#include <GL/gl.h>

#include "util/trace.h"

Rect<double> GrObjBBox::gnodeBoundingBox(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjBBox::withBorder");

  const Rect<double> native = child()->gnodeBoundingBox(canvas);

  // Do the object's internal scaling and alignment, and find the
  // bounding box in screen coordinates

  Rect<int> screen_pos;

  {
    Gfx::Canvas::StateSaver state(canvas);

    itsOwner->itsScaler.doScaling(canvas);
    itsOwner->itsAligner.doAlignment(canvas, native);

    screen_pos = canvas.getScreenFromWorld(native);
  }

  // Add a pixel border around the edges of the image...
  int bp = pixelBorder();

  screen_pos.widenByStep(bp);
  screen_pos.heightenByStep(bp);

  // ... and project back to world coordinates
  return canvas.getWorldFromScreen(screen_pos);
}

void GrObjBBox::gnodeDraw(Gfx::Canvas& canvas) const
{
DOTRACE("GrObjBBox::gnodeDraw");

  if (!itsIsVisible) return;

  Rect<double> bounds = child()->gnodeBoundingBox(canvas);

  glPushAttrib(GL_LINE_BIT);
  {
    glLineWidth(1.0);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x0F0F);

    glBegin(GL_LINE_LOOP);
      glVertex2d(bounds.left(), bounds.bottom());
      glVertex2d(bounds.right(), bounds.bottom());
      glVertex2d(bounds.right(), bounds.top());
      glVertex2d(bounds.left(), bounds.top());
    glEnd();
  }
  glPopAttrib();
}

void GrObjBBox::gnodeUndraw(Gfx::Canvas& canvas) const
{
  gnodeDraw(canvas);
}

static const char vcid_grobjbbox_cc[] = "$Header$";
#endif // !GROBJBBOX_CC_DEFINED
