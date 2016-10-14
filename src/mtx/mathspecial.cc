/** @file mtx/mathspecial.cc math special functions (gammaln(),
    erfc(), etc.) */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Mar  8 16:28:26 2001 (split from num.h)
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

#ifndef GROOVX_PKGS_MTX_MATHSPECIAL_CC_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_MATHSPECIAL_CC_UTC20050626084022_DEFINED

#include "mathspecial.h"

#include "rutz/trace.h"

double dash::detail::gammaln_engine(const double xx)
{
GVX_TRACE("dash::detail::gammaln_engine");

  static const double cof[6] =
    {
      76.18009172947146,     -86.50532032941677,
      24.01409824083091,     -1.231738572450155,
      0.1208650973866179e-2, -0.5395239384953e-5
    };

  double ser=1.000000000190015;

  for (int j = 0; j < 5; ++j)
    ser += cof[j]/(xx+j+1);

  const double tmp1 = xx+5.5;
  const double tmp = tmp1 - (xx+0.5)*log(tmp1);

  return -tmp+log(2.5066282746310005*ser/xx);
}

#endif // !GROOVX_PKGS_MTX_MATHSPECIAL_CC_UTC20050626084022_DEFINED
