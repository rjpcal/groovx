///////////////////////////////////////////////////////////////////////
//
// gbcolor.h
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Dec  2 13:09:31 2000
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

#ifndef GROOVX_GFX_GBCOLOR_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GBCOLOR_H_UTC20050626084024_DEFINED

#include "rutz/multivalue.h"

#include "gfx/rgbacolor.h"

/// Field class for RGBA colors.
class GbColor : public Gfx::RgbaColor, public rutz::multi_value<double>
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

  virtual rutz::fstring value_typename() const;

  virtual const double* const_begin() const;
};

static const char vcid_groovx_gfx_gbcolor_h_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GBCOLOR_H_UTC20050626084024_DEFINED
