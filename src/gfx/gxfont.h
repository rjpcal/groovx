///////////////////////////////////////////////////////////////////////
//
// gxfont.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Nov 12 18:34:57 2002
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

#ifndef GXFONT_H_DEFINED
#define GXFONT_H_DEFINED

template <class T> class shared_ptr;

namespace Gfx
{
  class Bbox;
  class Canvas;
  template <class V> class Rect;
}

/// An abstract class for fonts.
class GxFont
{
public:
  /// Factory function.
  static shared_ptr<GxFont> make(const char* name);

  /// Virtual destructor.
  virtual ~GxFont() throw();

  /// Get the string name of the font.
  virtual const char* fontName() const = 0;

  /// Get an OpenGL display list for rendering the font.
  virtual unsigned int listBase() const = 0;

  /// Get the bounding box of a piece of text rendered with this font.
  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const = 0;
};

static const char vcid_gxfont_h[] = "$Header$";
#endif // !GXFONT_H_DEFINED
