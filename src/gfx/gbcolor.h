///////////////////////////////////////////////////////////////////////
//
// gbcolor.h
//
// Copyright (c) 2000-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Dec  2 13:09:31 2000
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

#ifndef GBCOLOR_H_DEFINED
#define GBCOLOR_H_DEFINED

#include "util/multivalue.h"

#include "gx/rgbacolor.h"

/// Field class for RGBA colors.
class GbColor : public Gfx::RgbaColor, public TMultiValue<double>
{
public:
  /// Construct with a given luminance (default = white).
  GbColor(double v = 1.0);

  /// Construct with given red/green/blue/alpha values.
  GbColor(double r, double g, double b, double a);
  virtual ~GbColor();

  /// Get the underlying color value.
        Gfx::RgbaColor& color()       { return *this; }
  /// Get the underlying color value.
  const Gfx::RgbaColor& color() const { return *this; }

  virtual fstring getNativeTypeName() const;

  virtual const double* constBegin() const;
};

static const char vcid_gbcolor_h[] = "$Header$";
#endif // !GBCOLOR_H_DEFINED
