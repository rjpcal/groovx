///////////////////////////////////////////////////////////////////////
//
// grobjbbox.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 09:06:14 2001
// written: Fri Aug 24 16:11:36 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJBBOX_H_DEFINED
#define GROBJBBOX_H_DEFINED

#include "gnode.h"

#include "tcl/tcltimer.h"

class GrObjBBox : public Gnode {
private:
  GrObjBBox(const GrObjBBox&);
  GrObjBBox& operator=(const GrObjBBox&);

public:
  GrObjBBox(shared_ptr<Gnode> child, Util::Signal& sig) :
    Gnode(child),
    itsIsVisible(false),
    itsPixelBorder(4),
    itsTimer(100, true)
  {
    itsTimer.sigTimeOut.connect(sig.slot());
  }

  bool isVisible() const { return itsIsVisible; }
  void setVisible(bool val)
  {
    itsIsVisible = val;

#ifdef ANIMATE_BBOX
    if (itsIsVisible)
      itsTimer.schedule();
    else
      itsTimer.cancel();
#endif
  }

  void setPixelBorder(int pixels) { itsPixelBorder = pixels; }
  int pixelBorder() const { return itsPixelBorder; }

  virtual void gnodeDraw(Gfx::Canvas& canvas) const;

  virtual Gfx::Rect<double> gnodeBoundingBox(Gfx::Canvas& canvas) const;

private:
  bool itsIsVisible;

  mutable int itsPixelBorder;

  Tcl::Timer itsTimer;
};

static const char vcid_grobjbbox_h[] = "$Header$";
#endif // !GROBJBBOX_H_DEFINED
