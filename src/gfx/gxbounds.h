///////////////////////////////////////////////////////////////////////
//
// gxbounds.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 13:34:26 2002
// written: Tue Nov 19 12:53:21 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXBOUNDS_H_DEFINED
#define GXBOUNDS_H_DEFINED

#include "gfx/gxbin.h"

class GxBounds : public GxBin
{
private:
  GxBounds(const GxBounds&);
  GxBounds& operator=(const GxBounds&);

public:
  GxBounds(Util::SoftRef<GxNode> child);

  virtual ~GxBounds();

  bool isVisible() const { return isItVisible; }
  void setVisible(bool val) { isItVisible = val; }

  void setPixelBorder(int pixels) { itsPixelBorder = pixels; }
  int pixelBorder() const { return itsPixelBorder; }

  virtual void readFrom(IO::Reader* /*reader*/) {};
  virtual void writeTo(IO::Writer* /*writer*/) const {};

  virtual void draw(Gfx::Canvas& canvas) const;

  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

private:
  bool isItVisible;

  int itsPixelBorder;

  mutable unsigned short itsStipple;
  mutable unsigned short itsMask;
};

static const char vcid_gxbounds_h[] = "$Header$";
#endif // !GXBOUNDS_H_DEFINED
