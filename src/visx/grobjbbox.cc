///////////////////////////////////////////////////////////////////////
//
// grobjbbox.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 10:45:53 2001
// written: Fri Aug 31 09:02:45 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJBBOX_CC_DEFINED
#define GROBJBBOX_CC_DEFINED

#include "grobjbbox.h"

#include "gfx/canvas.h"
#include "gfx/rect.h"
#include "gfx/rgbacolor.h"

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

unsigned int GrObjBBox::theirTimerCount = 0;
Tcl::Timer GrObjBBox::theirTimer(100, true);

namespace
{
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

GrObjBBox::GrObjBBox(Util::SoftRef<Gnode> child, Util::Signal& sig) :
  Gnode(child),
  isItVisible(false),
  itsPixelBorder(4),
  itsStipple(0x0F0F), // 0000111100001111
  itsMask(0x3333)     // 0011001100110011
{
  theirTimer.sigTimeOut.connect(sig.slot());
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

  child()->gnodeDraw(canvas);

  if (isItVisible)
    {
      Gfx::Rect<double> bounds =
        addPixelBorder(canvas,
                       child()->gnodeBoundingBox(canvas),
                       itsPixelBorder);

#ifdef ANIMATE_BBOX
      itsStipple ^= itsMask;

      itsMask = ~itsMask;
#endif

      Gfx::Canvas::AttribSaver saver(canvas);

      canvas.setColor(Gfx::RgbaColor(0.0, 0.0, 1.0, 1.0));

      canvas.setLineWidth(1.0);
      canvas.setLineStipple(itsStipple);

      canvas.drawRect(bounds);

      canvas.setPointSize(4.0);

      {
        Gfx::Canvas::PointsBlock block(canvas);

        canvas.vertex2(bounds.bottomLeft());
        canvas.vertex2(bounds.bottomRight());
        canvas.vertex2(bounds.topRight());
        canvas.vertex2(bounds.topLeft());
        canvas.vertex2(bounds.center());
      }
    }
}

static const char vcid_grobjbbox_cc[] = "$Header$";
#endif // !GROBJBBOX_CC_DEFINED
