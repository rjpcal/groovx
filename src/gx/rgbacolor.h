///////////////////////////////////////////////////////////////////////
//
// rgbacolor.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  2 13:08:26 2000
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RGBACOLOR_H_DEFINED
#define RGBACOLOR_H_DEFINED

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

static const char vcid_rgbacolor_h[] = "$Header$";
#endif // !RGBACOLOR_H_DEFINED
