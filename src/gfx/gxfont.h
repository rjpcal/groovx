/** @file gfx/gxfont.h abstract class for fonts */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Nov 12 18:34:57 2002
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

#ifndef GROOVX_GFX_GXFONT_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXFONT_H_UTC20050626084023_DEFINED

namespace geom
{
  template <class V> class rect;
}

namespace Gfx
{
  class Bbox;
  class Canvas;
}

/// An abstract class for fonts.
class GxFont
{
public:
  /// Virtual destructor.
  virtual ~GxFont() noexcept;

  /// Get the string name of the font.
  virtual const char* fontName() const = 0;

  /// Get an OpenGL display list for rendering the font.
  virtual unsigned int listBase() const = 0;

  /// Get the bounding box of a piece of text rendered with this font.
  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const = 0;

  /// Draw the given text on the canvas.
  virtual void drawText(const char* text, Gfx::Canvas& canvas) const = 0;
};

#endif // !GROOVX_GFX_GXFONT_H_UTC20050626084023_DEFINED
