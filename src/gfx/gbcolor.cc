/** @file gfx/gbcolor.cc field class for RGBA color values */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Sat Dec  2 13:10:25 2000
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

#include "gfx/gbcolor.h"

#include "rutz/fstring.h"

GbColor::GbColor(float v) :
  Gfx::RgbaColor(v),
  rutz::multi_value<float>(4)
{}

GbColor::GbColor(float r, float g, float b, float a) :
  Gfx::RgbaColor(r,g,b,a),
  rutz::multi_value<float>(4)
{}

GbColor::~GbColor() {}

//
// rutz::value interface
//

rutz::fstring GbColor::value_typename() const
{
  static rutz::fstring name("GbColor");
  return name;
}

const float* GbColor::const_begin() const
{
  return Gfx::RgbaColor::data();
}
