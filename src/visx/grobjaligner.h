///////////////////////////////////////////////////////////////////////
//
// grobjaligner.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 15:42:42 2001
// written: Fri Aug 10 14:59:14 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJALIGNER_H_DEFINED
#define GROBJALIGNER_H_DEFINED

#include "gmodes.h"
#include "gnode.h"
#include "point.h"

class GrObjAligner : public Gnode {
public:
  GrObjAligner(shared_ptr<Gnode> child) :
	 Gnode(child),
    itsMode(Gmodes::NATIVE_ALIGNMENT),
    itsCenter(0.0, 0.0)
  {}

  Point<double> getCenter(double width, double height) const
  {
    if (Gmodes::CENTER_ON_CENTER == itsMode)
      return Point<double>(0.0, 0.0);
    else if (Gmodes::NW_ON_CENTER == itsMode)
      return Point<double>(width/2.0, -height/2.0);
    else if (Gmodes::NE_ON_CENTER == itsMode)
      return Point<double>(-width/2.0, -height/2.0);
    else if (Gmodes::SW_ON_CENTER == itsMode)
      return Point<double>(width/2.0, height/2.0);
    else if (Gmodes::SE_ON_CENTER == itsMode)
      return Point<double>(-width/2.0, height/2.0);
    // else Gmodes::ARBITRARY_ON_CENTER:
    return itsCenter;
  }

  Gmodes::AlignmentMode getMode() const { return itsMode; }

  void setMode(Gmodes::AlignmentMode new_mode)
  {
    itsMode = new_mode;
  }

  virtual void gnodeDraw(Gfx::Canvas& canvas) const;

  virtual void gnodeUndraw(Gfx::Canvas& canvas) const;

  virtual Rect<double> gnodeBoundingBox(Gfx::Canvas& canvas) const;

private:
  void doAlignment(Gfx::Canvas& canvas, const Rect<double>& native) const;

public:
  Gmodes::AlignmentMode itsMode;
  Point<double> itsCenter;
};


static const char vcid_grobjaligner_h[] = "$Header$";
#endif // !GROBJALIGNER_H_DEFINED
