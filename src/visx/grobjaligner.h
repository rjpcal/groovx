///////////////////////////////////////////////////////////////////////
//
// grobjaligner.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 15:42:42 2001
// written: Fri Aug 10 10:46:48 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJALIGNER_H_DEFINED
#define GROBJALIGNER_H_DEFINED

#include "grobj.h"
#include "point.h"

class GrObjAligner {
public:
  GrObjAligner() :
    itsMode(GrObj::NATIVE_ALIGNMENT),
    itsCenter(0.0, 0.0)
    {}

  Point<double> getCenter(double width, double height) const
  {
    if (GrObj::CENTER_ON_CENTER == itsMode)
      return Point<double>(0.0, 0.0);
    else if (GrObj::NW_ON_CENTER == itsMode)
      return Point<double>(width/2.0, -height/2.0);
    else if (GrObj::NE_ON_CENTER == itsMode)
      return Point<double>(-width/2.0, -height/2.0);
    else if (GrObj::SW_ON_CENTER == itsMode)
      return Point<double>(width/2.0, height/2.0);
    else if (GrObj::SE_ON_CENTER == itsMode)
      return Point<double>(-width/2.0, height/2.0);
    // else GrObj::ARBITRARY_ON_CENTER:
    return itsCenter;
  }

  void doAlignment(Gfx::Canvas& canvas, const Rect<double>& native) const;

  void setMode(GrObj::AlignmentMode new_mode)
  {
    itsMode = new_mode;
  }

  GrObj::AlignmentMode getMode() const { return itsMode; }

  GrObj::AlignmentMode itsMode;
  Point<double> itsCenter;
};


static const char vcid_grobjaligner_h[] = "$Header$";
#endif // !GROBJALIGNER_H_DEFINED
