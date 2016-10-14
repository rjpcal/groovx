/** @file gfx/rgbacolor.h RGBA color values (red/green/blue/alpha) */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Dec  2 13:08:26 2000
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

#ifndef GROOVX_GFX_RGBACOLOR_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_RGBACOLOR_H_UTC20050626084023_DEFINED

namespace Gfx
{
  class RgbaColor;
}

/// RGBA (red/green/blue/alpha) color class.
class Gfx::RgbaColor
{
private:
  float itsData[4];

public:
  RgbaColor(float v = 1.0)
    { itsData[0] = itsData[1] = itsData[2] = itsData[3] = v; }

  RgbaColor(float r_, float g_, float b_, float a_)
    { set(r_, g_, b_, a_); }

        float& r()       { return itsData[0]; }
  const float& r() const { return itsData[0]; }

        float& g()       { return itsData[1]; }
  const float& g() const { return itsData[1]; }

        float& b()       { return itsData[2]; }
  const float& b() const { return itsData[2]; }

        float& a()       { return itsData[3]; }
  const float& a() const { return itsData[3]; }

  void get(float& r_, float& g_, float& b_, float& a_) const
    { r_ = r(); g_ = g(); b_ = b(); a_ = a(); }

  void set(float r_, float g_, float b_, float a_)
    { r() = r_; g() = g_; b() = b_; a() = a_; }

        float* data()       { return &itsData[0]; }
  const float* data() const { return &itsData[0]; }
};

static const char __attribute__((used)) vcid_groovx_gfx_rgbacolor_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_RGBACOLOR_H_UTC20050626084023_DEFINED
