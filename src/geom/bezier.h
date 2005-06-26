///////////////////////////////////////////////////////////////////////
//
// bezier.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 20 12:17:07 2002
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

#ifndef BEZIER_H_DEFINED
#define BEZIER_H_DEFINED

#include "util/algo.h"
#include "util/arrays.h"

#include <cmath>

namespace geom
{
  //  #######################################################
  //  =======================================================

  /// geom::bezier is a general class for evaluating Bezier curves
  /** Also for evaluation Bezier derivatives, and their minimum and
      maximum values. */

  class bezier
  {
  private:
    rutz::dynamic_block<double> m_ctrl;      // control points

    // coefficients of the Bezier polynomial
    // p(u) = c0[0] + c0[1]*u + c0[2]*u^2 + c0[3]*u^3
    rutz::dynamic_block<double> m_c0;

    // coefficients of the derivative polynomial
    // p'(u) = c1[0] + c1[1]*u + c1[2]*u^2
    rutz::dynamic_block<double> m_c1;

    double m_arg_min;  // value of u in [0,1] which gives the minimum
    double m_arg_max;  // value of u in [0,1] which gives the maximum
    double m_val_min;
    double m_val_max;

    static int choose(int n, int k)
    {
      // result = n! / [k!(n-k)!]
      int result = 1;

      // we have symmetry of (n k) = (n n-k)
      // smaller k gives a faster computation, so:
      if ( (n-k) < k ) { k = n-k; }

      // compute n! / (n-k)!  =  n*(n-1)*(n-2)...(n-k+1)
      for (int f = n; f > (n-k); --f)
        {
          result *= f;
        }
      for (int d = 2; d <= k; ++d)
        {
          result /= d;
        }

      return result;
    }

  public:
    /** Construct from a vector of control points. The argument
        extrema_res is used to specify the number of subdivisions that
        will be examined when searching for the extrema. If extrema_res
        is < 0 (the default value), a default number of subdivisions
        will be used. */
    bezier(const rutz::dynamic_block<double>& RR, int extrema_res=-1);

    /// Reset the control points,
    void set_control_points(const rutz::dynamic_block<double>& RR,
                            int extrema_res=-1);


    /// Evaluate the Bezier curve for u in [0, 1]
    double eval(double u);
    /// Evaluate the derivative of the Bezier curve for u in [0, 1]
    double eval_deriv(double u);

    /// Get the arg-min value of the Bezier curve in the range [0, 1]
    double arg_min() { return m_arg_min; }
    /// Get the arg-max value of the Bezier curve in the range [0, 1]
    double arg_max() { return m_arg_max; }

    /// Get the min value of the Bezier curve in the range [0, 1]
    double eval_min() { return m_val_min; }
    /// Get the max value of the Bezier curve in the range [0, 1]
    double eval_max() { return m_val_max; }
  };

} // end namespace geom

//  #######################################################
//  =======================================================

geom::bezier::bezier(const rutz::dynamic_block<double>& RR,
                     int extrema_res) :
  m_ctrl(),
  m_c0(),
  m_c1(),
  m_arg_min(0.0),
  m_arg_max(0.0),
  m_val_min(0.0),
  m_val_max(0.0)
{
  set_control_points(RR, extrema_res);
}

void geom::bezier::set_control_points
  (const rutz::dynamic_block<double>& RR,
   int extrema_res)
{
  m_ctrl = RR;
  m_c0.resize(RR.size());
  m_c1.resize(RR.size()-1);

  int n = RR.size()-1;          // degree of polynomial = num_points - 1

  //         ( n )    i  {     (i+k)   ( i )      }
  // c0    = (   ) * sum {  (-1)     * (   ) * R  }
  //   i     ( i )   k=0 {             ( k )    k }

  {for (int i = 0; i <= n; ++i)
    {
      m_c0[i] = 0;

      for (int k = 0; k <= i; ++k)
        {
          int i_choose_k = bezier::choose(i,k);

          m_c0[i] += ( (i+k)%2 ? 1 : -1) * i_choose_k * m_ctrl[k];
        }

      m_c0[i] *= bezier::choose(n,i);
    }
  }

  {for (size_t i = 0; i < m_c1.size(); ++i)
    {
      m_c1[i] = (i+1) * m_c0[i+1];
    }
  }


  if (extrema_res <= 0) { extrema_res = 4*(RR.size()); }

  m_arg_min = m_arg_max = 0.0;
  m_val_min = m_val_max = eval(0.0);

  for (int e = 1; e <= extrema_res; ++e)
    {
      const double u = double(e)/extrema_res;
      const double current = eval(u);
      if (current < m_val_min)
        {
          m_val_min = current;
          m_arg_min = u;
        }
      else if (current > m_val_max)
        {
          m_val_max = current;
          m_arg_max = u;
        }
    }
}

double geom::bezier::eval(double u)
{
  double result = 0.0;
  double powers = 1.0;
  for (size_t i = 0; i < m_c0.size(); ++i)
    {
      result += m_c0[i] * powers;
      powers *= u;
    }
  return result;
}

double geom::bezier::eval_deriv(double u)
{
  double result = 0.0;
  double powers = 1.0;
  for (size_t i = 0; i < m_c1.size(); ++i)
    {
      result += m_c1[i] * powers;
      powers *= u;
    }
  return result;
}

static const char vcid_bezier_h[] = "$Id$ $URL$";
#endif // !BEZIER_H_DEFINED
