///////////////////////////////////////////////////////////////////////
//
// grobjbbox.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 09:06:14 2001
// written: Fri Aug 10 10:56:09 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJBBOX_H_DEFINED
#define GROBJBBOX_H_DEFINED

#include "rect.h"

#include "gfx/canvas.h"

class GrObjImpl;

class GrObjBBox {
private:
  GrObjBBox(const GrObjBBox&);
  GrObjBBox& operator=(const GrObjBBox&);

public:
  GrObjBBox(const GrObjImpl* owner) :
    itsIsVisible(false),
    itsOwner(owner),
    itsPixelBorder(4)
  {}

  Rect<double> withBorder(const Rect<double>& native,
                          Gfx::Canvas& canvas) const;

  bool isVisible() const { return itsIsVisible; }
  void setVisible(bool val) { itsIsVisible = val; }

  void setPixelBorder(int pixels) { itsPixelBorder = pixels; }
  int pixelBorder() const { return itsPixelBorder; }

  void draw(Rect<double>& bounds, Gfx::Canvas& canvas) const;

private:
  // data
  bool itsIsVisible;

  const GrObjImpl* const itsOwner;

  mutable int itsPixelBorder;
};

static const char vcid_grobjbbox_h[] = "$Header$";
#endif // !GROBJBBOX_H_DEFINED
