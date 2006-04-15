/** @file gfx/gxvectorfont.h a basic "blocky" vector-graphics font */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 13 20:05:23 2002
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

#ifndef GROOVX_GFX_GXVECTORFONT_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXVECTORFONT_H_UTC20050626084023_DEFINED

#include "gfx/gxfont.h"

/// A basic "blocky" vector-graphics font.
class GxVectorFont : public GxFont
{
public:
  GxVectorFont();

  virtual ~GxVectorFont() throw();

  virtual const char* fontName() const;

  virtual unsigned int listBase() const;

  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const;

  virtual void drawText(const char* text, Gfx::Canvas& canvas) const;

  /// Return the vector height of the font, in world coords.
  double vectorHeight() const;

private:
  GxVectorFont(const GxVectorFont&);
  GxVectorFont& operator=(const GxVectorFont&);
};

static const char __attribute__((used)) vcid_groovx_gfx_gxvectorfont_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXVECTORFONT_H_UTC20050626084023_DEFINED
