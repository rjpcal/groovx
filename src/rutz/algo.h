/** @file rutz/algo.h some trivial algos from <algorithm>, but much
    more lightweight */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sun Jul 22 23:31:48 2001
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

#ifndef GROOVX_RUTZ_ALGO_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_ALGO_H_UTC20050626084020_DEFINED

#include <algorithm>

namespace rutz
{
  template <class T>
  inline T clamp(const T& val, const T& lower, const T& upper)
  {
    return std::max(lower, std::min(upper, val));
  }
}

#endif // !GROOVX_RUTZ_ALGO_H_UTC20050626084020_DEFINED
