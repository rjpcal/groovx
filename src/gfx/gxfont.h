///////////////////////////////////////////////////////////////////////
//
// gxfont.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov 12 18:34:57 2002
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_GFX_GXFONT_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXFONT_H_UTC20050626084023_DEFINED

namespace rutz
{
  template <class T> class shared_ptr;
}

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
  /// Factory function.
  static rutz::shared_ptr<GxFont> make(const char* name);

  /// Virtual destructor.
  virtual ~GxFont() throw();

  /// Get the string name of the font.
  virtual const char* fontName() const = 0;

  /// Get an OpenGL display list for rendering the font.
  virtual unsigned int listBase() const = 0;

  /// Get the bounding box of a piece of text rendered with this font.
  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const = 0;

  /// Draw the given text on the canvas.
  virtual void drawText(const char* text, Gfx::Canvas& canvas) const = 0;
};

static const char vcid_groovx_gfx_gxfont_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXFONT_H_UTC20050626084023_DEFINED
