///////////////////////////////////////////////////////////////////////
//
// grobjbbox.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 09:06:14 2001
// written: Fri Aug 10 17:33:38 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJBBOX_H_DEFINED
#define GROBJBBOX_H_DEFINED

#include "gnode.h"

class GrObjBBox : public Gnode {
private:
  GrObjBBox(const GrObjBBox&);
  GrObjBBox& operator=(const GrObjBBox&);

public:
  GrObjBBox(shared_ptr<Gnode> child) :
    Gnode(child),
    itsIsVisible(false),
    itsPixelBorder(4)
  {}

  bool isVisible() const { return itsIsVisible; }
  void setVisible(bool val) { itsIsVisible = val; }

  void setPixelBorder(int pixels) { itsPixelBorder = pixels; }
  int pixelBorder() const { return itsPixelBorder; }

  virtual void gnodeDraw(Gfx::Canvas& canvas) const;
  virtual void gnodeUndraw(Gfx::Canvas& canvas) const;

  virtual Rect<double> gnodeBoundingBox(Gfx::Canvas& canvas) const;

private:
  bool itsIsVisible;

  mutable int itsPixelBorder;
};

static const char vcid_grobjbbox_h[] = "$Header$";
#endif // !GROBJBBOX_H_DEFINED
