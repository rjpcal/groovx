///////////////////////////////////////////////////////////////////////
//
// grobjaligner.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 15:48:47 2001
// written: Wed Jul 18 18:14:35 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJALIGNER_CC_DEFINED
#define GROBJALIGNER_CC_DEFINED

#include "grobjaligner.h"

#include "rect.h"

#include <GL/gl.h>

#include "util/trace.h"

void GrObjAligner::doAlignment(const Rect<double>& native) const
{
DOTRACE("GrObjAligner::doAlignment");

  if (GrObj::NATIVE_ALIGNMENT == itsMode) return;

  // This indicates where the center of the object will go
  Point<double> center = getCenter(native.width(), native.height());

  center -= native.center();

  glTranslated(center.x(), center.y(), 0.0);
}

static const char vcid_grobjaligner_cc[] = "$Header$";
#endif // !GROBJALIGNER_CC_DEFINED
