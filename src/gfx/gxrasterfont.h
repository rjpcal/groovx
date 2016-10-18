/** @file gfx/gxrasterfont.h build an OpenGL raster font from a
    platform-specific bitmap font */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 13 16:44:36 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_GFX_GXRASTERFONT_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXRASTERFONT_H_UTC20050626084023_DEFINED

#include "gfx/gxfont.h"

/// Builds an OpenGL raster font from an X11 font.
class GxRasterFont : public GxFont
{
public:
  virtual ~GxRasterFont() noexcept = 0;

  virtual const char* fontName() const = 0;

  virtual unsigned int listBase() const = 0;

  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const = 0;

  virtual void drawText(const char* text, Gfx::Canvas& canvas) const = 0;

  /// Return the line height of the font, in screen coords (i.e., pixels).
  virtual int rasterHeight() const = 0;
};

#endif // !GROOVX_GFX_GXRASTERFONT_H_UTC20050626084023_DEFINED
