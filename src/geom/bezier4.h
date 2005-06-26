///////////////////////////////////////////////////////////////////////
//
// bezier4.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 20 12:17:39 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef GROOVX_GEOM_BEZIER4_H_UTC20050626084023_DEFINED
#define GROOVX_GEOM_BEZIER4_H_UTC20050626084023_DEFINED

#include "rutz/algo.h"

#include <cmath>

namespace geom
{
  //  #######################################################
  //  =======================================================

  /// For evaluating Bezier curves with four control points.

  class bezier4
  {
  private:
    double m_ctrl[4];        // control points

    // coefficients of the Bezier polynomial
    // p(u) = c0[0] + c0[1]*u + c0[2]*u^2 + c0[3]*u^3
    double m_c0[4];

    // coefficients of the derivative polynomial
    // p'(u) = c1[0] + c1[1]*u + c1[2]*u^2
    double m_c1[4];

    double m_val_min;
    double m_val_max;

    bool m_have_min_max;

    void compute_min_max();

  public:
    /// Construct from four control points.
    inline bezier4(double R0, double R1, double R2, double R3);

    /// Reset the control points.
    inline void set_control_points(double R0, double R1,
                                   double R2, double R3);

    /// Evaluate the Bezier curve for u in [0, 1]
    inline double eval(double u);
    /// Evaluate the derivative of the Bezier curve for u in [0, 1]
    inline double eval_deriv(double u);

    /// Get the min value of the Bezier curve in the range [0, 1]
    inline double eval_min();
    /// Get the max value of the Bezier curve in the range [0, 1]
    inline double eval_max();
  };

} // end namespace geom


//  #######################################################
//  =======================================================

inline geom::bezier4::bezier4(double R0, double R1,
                              double R2, double R3) :
  m_ctrl(),
  m_c0(),
  m_c1(),
  m_val_min(0.0),
  m_val_max(0.0),
  m_have_min_max(false)
{
  set_control_points(R0, R1, R2, R3);
}

inline void geom::bezier4::compute_min_max()
{
  double b2_4ac = m_c1[1]*m_c1[1] - 4*m_c1[0]*m_c1[2];

  using rutz::max;
  using rutz::min;

  m_val_min = min(eval(0.0), eval(1.0));
  m_val_max = max(eval(0.0), eval(1.0));

  if (b2_4ac >= 0.0)
    {
      double part1 = -m_c1[1] / (2*m_c1[2]);
      double part2 = sqrt(b2_4ac) / (2*m_c1[2]);

      double extremum1 = rutz::clamp(part1 + part2, 0.0, 1.0);
      double extremum2 = rutz::clamp(part1 - part2, 0.0, 1.0);

      double v1 = eval(extremum1);
      double v2 = eval(extremum2);

      if      (v1 < m_val_min) m_val_min = v1;
      else if (v1 > m_val_max) m_val_max = v1;

      if      (v2 < m_val_min) m_val_min = v2;
      else if (v2 > m_val_max) m_val_max = v2;
    }

  m_have_min_max = true;
}

inline void geom::bezier4::set_control_points(double R0, double R1,
                                              double R2, double R3)
{
  m_have_min_max = false;

  m_ctrl[0] = R0;
  m_ctrl[1] = R1;
  m_ctrl[2] = R2;
  m_ctrl[3] = R3;

  // Compute polynomial coefficients
  m_c0[0] =    m_ctrl[0];
  m_c0[1] = -3*m_ctrl[0] + 3*m_ctrl[1];
  m_c0[2] =  3*m_ctrl[0] - 6*m_ctrl[1] + 3*m_ctrl[2];
  m_c0[3] =   -m_ctrl[0] + 3*m_ctrl[1] - 3*m_ctrl[2] + m_ctrl[3];

  // Compute polynomial coefficients of derivative
  m_c1[0] = 1 * m_c0[1];
  m_c1[1] = 2 * m_c0[2];
  m_c1[2] = 3 * m_c0[3];
}

inline double geom::bezier4::eval(double u)
{
  return (m_c0[0] + m_c0[1]*u + m_c0[2]*u*u + m_c0[3]*u*u*u);
}

inline double geom::bezier4::eval_deriv(double u)
{
  return (m_c1[0] + m_c1[1]*u + m_c1[2]*u*u);
}

inline double geom::bezier4::eval_max()
{
  if (!m_have_min_max) compute_min_max();
  return m_val_max;
}

inline double geom::bezier4::eval_min()
{
  if (!m_have_min_max) compute_min_max();
  return m_val_min;
}

static const char vcid_groovx_geom_bezier4_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GEOM_BEZIER4_H_UTC20050626084023_DEFINED
