/** @file gfx/gxcolor.h GxNode class for manipulating the current RGBA
    color */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Aug 22 17:42:48 2001
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

#ifndef GROOVX_GFX_GXCOLOR_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXCOLOR_H_UTC20050626084024_DEFINED

#include "gfx/gbcolor.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

/// GxColor is a graphics node class for RGBA colors.
class GxColor : public GxNode, public FieldContainer
{
private:
  GxColor(const GxColor&);
  GxColor& operator=(const GxColor&);

  GbColor rgbaColor;

protected:
  /// Default constructor.
  GxColor();

public:
  virtual ~GxColor() throw();

  /// Factory function.
  static GxColor* make() { return new GxColor; }

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

  /// Get GxColor's fields.
  static const FieldMap& classFields();

  /// This is a no-op since GxColor doesn't actually draw anything onscreen.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  virtual void draw(Gfx::Canvas& canvas) const;
};

#endif // !GROOVX_GFX_GXCOLOR_H_UTC20050626084024_DEFINED
