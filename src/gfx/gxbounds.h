///////////////////////////////////////////////////////////////////////
//
// gxbounds.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 13:34:26 2002
// commit: $Id$
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

/// A graphic object that draws a boundary around its child object.
class GxBounds : public GxBin
{
private:
  GxBounds(const GxBounds&);
  GxBounds& operator=(const GxBounds&);

public:
  /// Construct with a given child object.
  GxBounds(Util::SoftRef<GxNode> child);

  /// Virtual destructor.
  virtual ~GxBounds() throw();

  /// Query whether the rect boundary should be drawn.
  bool isVisible() const { return isItVisible; }

  /// Set the visibility of the rect boundary.
  void setVisible(bool val) { isItVisible = val; }

  /// Query the gap between the child object and the boundary.
  int percentBorder() const { return itsPercentBorder; }

  /// Change the gap between the child object and the boundary.
  void setPercentBorder(int pixels) { itsPercentBorder = pixels; }

  virtual void readFrom(IO::Reader& /*reader*/) {};
  virtual void writeTo(IO::Writer& /*writer*/) const {};

  virtual void draw(Gfx::Canvas& canvas) const;

  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

private:
  bool isItVisible;

  int itsPercentBorder;

  mutable unsigned short itsStipple;
  mutable unsigned short itsMask;
};

static const char vcid_gxbounds_h[] = "$Header$";
#endif // !GXBOUNDS_H_DEFINED
