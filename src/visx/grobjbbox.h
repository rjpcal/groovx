///////////////////////////////////////////////////////////////////////
//
// grobjbbox.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 09:06:14 2001
// written: Fri Jan 18 16:06:52 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJBBOX_H_DEFINED
#define GROBJBBOX_H_DEFINED

#include "visx/gnode.h"

#include "gx/rect.h"

class GrObjBBox : public Gnode
{
private:
  GrObjBBox(const GrObjBBox&);
  GrObjBBox& operator=(const GrObjBBox&);

public:
  GrObjBBox(Util::SoftRef<Gnode> child);

  virtual ~GrObjBBox();

  bool isVisible() const { return isItVisible; }
  void setVisible(bool val) { isItVisible = val; }

  void setPixelBorder(int pixels) { itsPixelBorder = pixels; }
  int pixelBorder() const { return itsPixelBorder; }

  virtual void gnodeDraw(Gfx::Canvas& canvas) const;

  virtual Gfx::Rect<double> gnodeBoundingBox(Gfx::Canvas& canvas) const;

private:
  bool isItVisible;

  int itsPixelBorder;

  mutable unsigned short itsStipple;
  mutable unsigned short itsMask;
};

static const char vcid_grobjbbox_h[] = "$Header$";
#endif // !GROBJBBOX_H_DEFINED
