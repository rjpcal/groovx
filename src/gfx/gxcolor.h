///////////////////////////////////////////////////////////////////////
//
// gxcolor.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Aug 22 17:42:48 2001
// written: Thu Apr  3 15:32:28 2003
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

#ifndef GXCOLOR_H_DEFINED
#define GXCOLOR_H_DEFINED

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
  virtual ~GxColor();

  /// Factory function.
  static GxColor* make() { return new GxColor; }

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  /// Get GxColor's fields.
  static const FieldMap& classFields();

  /// This is a no-op since GxColor doesn't actually draw anything onscreen.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  virtual void draw(Gfx::Canvas& canvas) const;
};

static const char vcid_gxcolor_h[] = "$Header$";
#endif // !GXCOLOR_H_DEFINED
