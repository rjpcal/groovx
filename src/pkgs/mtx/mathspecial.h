/** @file pkgs/mtx/mathspecial.h math special functions (gammaln(),
    erfc(), etc.) */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Mar  8 16:27:36 2001 (split from num.h)
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

#ifndef GROOVX_PKGS_MTX_MATHSPECIAL_H_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_MATHSPECIAL_H_UTC20050626084022_DEFINED

#include <cmath>

/// Numeric operations.
namespace dash
{
  namespace detail
  {
    double gammaln_engine(double xx);
  }

  /// natural-log of the gamma function
  inline double gammaln(double xx)
  {
    static bool filled = false;
    static const int TABLE_SIZE = 101;
    static double lookup[TABLE_SIZE] = { 0.0 };


    const double tol = 1e-50;

    const int ival = int(xx);

    if ( (ival < TABLE_SIZE) && (xx - double(ival) < tol) )
      {
        if (!filled)
          {
            for (int i = 0; i < TABLE_SIZE; ++i)
              lookup[i] = dash::detail::gammaln_engine(double(i));

            filled = true;
          }
        return lookup[ival];
      }
    else
      {
        return dash::detail::gammaln_engine(xx);
      }
  }

  /// the error function
  inline double erfc(const double x)
  {
    const double z = fabs(x);

    const double t = 1.0/(1.0+0.5*fabs(x));

    const double ans =
      t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
      t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
      t*(-0.82215223+t*0.17087277)))))))))
      ;

    return x >= 0.0 ? ans : 2.0-ans;
  }
}

static const char vcid_groovx_pkgs_mtx_mathspecial_h_utc20050626084022[] = "$Id$ $HeadURL$";
#endif // !GROOVX_PKGS_MTX_MATHSPECIAL_H_UTC20050626084022_DEFINED
