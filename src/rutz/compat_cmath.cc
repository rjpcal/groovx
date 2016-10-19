/** @file rutz/compat_cmath.cc Compatibility definitions for things missing from <cmath> */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jan 16 18:18:40 2006
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

#include "rutz/compat_cmath.h"

#ifdef MISSING_M_PI

#include <cmath>

namespace
{
  const double m_pi_value           = 4.0 * atan(1.0);
  const double m_pi_2_value         = 2.0 * atan(1.0);
  const double m_e_value            = exp(1.0);
  const double m_ln2_value      = log(2.0);
}

double rutz::compat_m_pi()   { return m_pi_value; }
double rutz::compat_m_pi_2() { return m_pi_2_value; }
double rutz::compat_m_e()    { return m_e_value; }
double rutz::compat_m_ln2()  { return m_ln2_value; }

#endif
