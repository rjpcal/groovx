///////////////////////////////////////////////////////////////////////
//
// rgbacolor.h
//
// Copyright (c) 2000-2005
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
  double itsData[4];

public:
  RgbaColor(double v = 1.0)
    { itsData[0] = itsData[1] = itsData[2] = itsData[3] = v; }

  RgbaColor(double r_, double g_, double b_, double a_)
    { set(r_, g_, b_, a_); }

        double& r()       { return itsData[0]; }
  const double& r() const { return itsData[0]; }

        double& g()       { return itsData[1]; }
  const double& g() const { return itsData[1]; }

        double& b()       { return itsData[2]; }
  const double& b() const { return itsData[2]; }

        double& a()       { return itsData[3]; }
  const double& a() const { return itsData[3]; }

  void get(double& r_, double& g_, double& b_, double& a_) const
    { r_ = r(); g_ = g(); b_ = b(); a_ = a(); }

  void set(double r_, double g_, double b_, double a_)
    { r() = r_; g() = g_; b() = b_; a() = a_; }

        double* data()       { return &itsData[0]; }
  const double* data() const { return &itsData[0]; }
};

static const char vcid_groovx_gfx_rgbacolor_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_RGBACOLOR_H_UTC20050626084023_DEFINED
