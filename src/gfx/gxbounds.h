///////////////////////////////////////////////////////////////////////
//
// gxbounds.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 13:34:26 2002
// written: Wed Mar 19 17:56:04 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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
