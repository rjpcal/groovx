///////////////////////////////////////////////////////////////////////
//
// grobjbbox.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 10:45:53 2001
// written: Thu Jul 19 11:08:42 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJBBOX_CC_DEFINED
#define GROBJBBOX_CC_DEFINED

#include "grobjbbox.h"

#include "grobjimpl.h"

#include <GL/gl.h>

#include "util/trace.h"

Rect<double> GrObjBBox::withBorder(
  const Rect<double>& native,
  GWT::Canvas& canvas) const
{
DOTRACE("GrObjBBox::withBorder");

  // Do the object's internal scaling and alignment, and find the
  // bounding box in screen coordinates

  Rect<int> screen_pos;

  {
    GWT::Canvas::StateSaver state(canvas);

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

void GrObjBBox::draw(Rect<double>& bounds, GWT::Canvas& canvas) const
{
DOTRACE("GrObjBBox::draw");

  if (!itsIsVisible) return;

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

static const char vcid_grobjbbox_cc[] = "$Header$";
#endif // !GROBJBBOX_CC_DEFINED
