///////////////////////////////////////////////////////////////////////
//
// gxrasterfont.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 16:44:36 2002
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

#ifndef GXRASTERFONT_H_DEFINED
#define GXRASTERFONT_H_DEFINED

#include "gfx/gxfont.h"

/// Builds an OpenGL raster font from an X11 font.
class GxRasterFont : public GxFont
{
public:
  /// Construct from the given X11 font specification.
  GxRasterFont(const char* fontname);

  virtual ~GxRasterFont() throw();

  virtual const char* fontName() const;

  virtual unsigned int listBase() const;

  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const;

  virtual void drawText(const char* text, Gfx::Canvas& canvas) const;

  /// Return the line height of the font, in screen coords (i.e., pixels).
  int rasterHeight() const;

private:
  GxRasterFont(const GxRasterFont&);
  GxRasterFont& operator=(const GxRasterFont&);

  struct Impl;
  Impl* rep;
};

static const char vcid_gxrasterfont_h[] = "$Header$";
#endif // !GXRASTERFONT_H_DEFINED
