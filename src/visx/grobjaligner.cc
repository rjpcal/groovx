///////////////////////////////////////////////////////////////////////
//
// grobjaligner.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 15:48:47 2001
// written: Thu Jul 19 10:33:33 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJALIGNER_CC_DEFINED
#define GROBJALIGNER_CC_DEFINED

#include "grobjaligner.h"

#include "rect.h"

#include "gwt/canvas.h"
#include "gx/vec3.h"

#include "util/trace.h"

void GrObjAligner::doAlignment(GWT::Canvas& canvas,
                               const Rect<double>& native) const
{
DOTRACE("GrObjAligner::doAlignment");

  if (GrObj::NATIVE_ALIGNMENT == itsMode) return;

  // This indicates where the center of the object will go
  Point<double> center = getCenter(native.width(), native.height());

  center -= native.center();

  Vec3<double> vec(center.x(), center.y(), 0.0);

  canvas.translate(vec);
}

static const char vcid_grobjaligner_cc[] = "$Header$";
#endif // !GROBJALIGNER_CC_DEFINED
