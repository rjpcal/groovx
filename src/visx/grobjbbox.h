///////////////////////////////////////////////////////////////////////
//
// grobjbbox.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 09:06:14 2001
// written: Sat Aug 25 12:54:35 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJBBOX_H_DEFINED
#define GROBJBBOX_H_DEFINED

#include "gnode.h"

#include "gfx/rect.h"

#include "tcl/tcltimer.h"

#define ANIMATE_BBOX

class GrObjBBox : public Gnode
{
private:
  GrObjBBox(const GrObjBBox&);
  GrObjBBox& operator=(const GrObjBBox&);

public:
  GrObjBBox(Util::SoftRef<Gnode> child, Util::Signal& sig);

  virtual ~GrObjBBox() {}

  bool isVisible() const { return isItVisible; }
  void setVisible(bool val)
  {
	 if (isItVisible == val) return;

    isItVisible = val;

#ifdef ANIMATE_BBOX
    if (isItVisible)
		{
		  ++theirTimerCount;
		  if (theirTimerCount == 1)
			 theirTimer.schedule();
		}
    else
		{
		  --theirTimerCount;
		  if (theirTimerCount == 0)
			 theirTimer.cancel();
		}
#endif
  }

  void setPixelBorder(int pixels) { itsPixelBorder = pixels; }
  int pixelBorder() const { return itsPixelBorder; }

  virtual void gnodeDraw(Gfx::Canvas& canvas) const;

  virtual Gfx::Rect<double> gnodeBoundingBox(Gfx::Canvas& canvas) const;

private:
  bool isItVisible;

  mutable int itsPixelBorder;

  static unsigned int theirTimerCount;
  static Tcl::Timer theirTimer;

  mutable unsigned short itsStipple;
  mutable unsigned short itsMask;
};

static const char vcid_grobjbbox_h[] = "$Header$";
#endif // !GROBJBBOX_H_DEFINED
