///////////////////////////////////////////////////////////////////////
//
// grobjaligner.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 18 15:42:42 2001
// written: Wed Nov 13 10:21:42 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJALIGNER_H_DEFINED
#define GROBJALIGNER_H_DEFINED

#include "visx/gmodes.h"
#include "visx/gnode.h"

#include "gx/rect.h"
#include "gx/vec2.h"

/// Controls how a GrObj is aligned relative to the origin when it is drawn.
class GrObjAligner : public Gnode
{
public:
  GrObjAligner(Util::SoftRef<Gnode> child) :
    Gnode(child),
    itsMode(Gmodes::NATIVE_ALIGNMENT),
    itsCenter(0.0, 0.0)
  {}

  Gfx::Vec2<double> getCenter(const Gfx::Rect<double>& bounds) const
  {
    switch (itsMode)
      {
      case Gmodes::CENTER_ON_CENTER:
        return Gfx::Vec2<double>(0.0, 0.0);
      case Gmodes::NW_ON_CENTER:
        return Gfx::Vec2<double>(bounds.width()/2.0, -bounds.height()/2.0);
      case Gmodes::NE_ON_CENTER:
        return Gfx::Vec2<double>(-bounds.width()/2.0, -bounds.height()/2.0);
      case Gmodes::SW_ON_CENTER:
        return Gfx::Vec2<double>(bounds.width()/2.0, bounds.height()/2.0);
      case Gmodes::SE_ON_CENTER:
        return Gfx::Vec2<double>(-bounds.width()/2.0, bounds.height()/2.0);
      case Gmodes::ARBITRARY_ON_CENTER:
        return itsCenter;
      }
    // NATIVE_ALIGNMENT
    return bounds.center();
  }

  Gmodes::AlignmentMode getMode() const { return itsMode; }

  void setMode(Gmodes::AlignmentMode new_mode)
  {
    itsMode = new_mode;
  }

  virtual void draw(Gfx::Canvas& canvas) const;

  virtual Gfx::Box<double> gnodeBoundingBox(Gfx::Canvas& canvas) const;

private:
  void doAlignment(Gfx::Canvas& canvas, const Gfx::Rect<double>& native) const;

public:
  Gmodes::AlignmentMode itsMode;
  Gfx::Vec2<double> itsCenter;
};


static const char vcid_grobjaligner_h[] = "$Header$";
#endif // !GROBJALIGNER_H_DEFINED
