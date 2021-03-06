/** @file gfx/gxbounds.h GxBin subclass that draws a 2-D boundary
    around its child object */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Nov 13 13:34:26 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_GFX_GXBOUNDS_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXBOUNDS_H_UTC20050626084024_DEFINED

#include "gfx/gxbin.h"

/// A graphic object that draws a boundary around its child object.
class GxBounds : public GxBin
{
private:
  GxBounds(const GxBounds&);
  GxBounds& operator=(const GxBounds&);

public:
  /// Construct with a given child object.
  GxBounds(nub::soft_ref<GxNode> child);

  /// Virtual destructor.
  virtual ~GxBounds() noexcept;

  /// Query whether the rect boundary should be drawn.
  bool isVisible() const { return isItVisible; }

  /// Set the visibility of the rect boundary.
  void setVisible(bool val) { isItVisible = val; }

  /// Query the gap between the child object and the boundary.
  int percentBorder() const { return itsPercentBorder; }

  /// Change the gap between the child object and the boundary.
  void setPercentBorder(int pixels) { itsPercentBorder = pixels; }

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  virtual void draw(Gfx::Canvas& canvas) const override;

  virtual void getBoundingCube(Gfx::Bbox& bbox) const override;

private:
  bool isItVisible;
  bool isItAnimated;
  int itsPercentBorder;

  mutable unsigned short itsStipple;
  mutable unsigned short itsMask;
};

#endif // !GROOVX_GFX_GXBOUNDS_H_UTC20050626084024_DEFINED
