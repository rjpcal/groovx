///////////////////////////////////////////////////////////////////////
//
// argspec.h
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun 27 16:47:04 2005
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

#ifndef GROOVX_TCL_ARGSPEC_H_UTC20050628064704_DEFINED
#define GROOVX_TCL_ARGSPEC_H_UTC20050628064704_DEFINED

#include <limits>

namespace Tcl
{
  struct ArgSpec
  {
    ArgSpec()
      :
      nargMin(0),
      nargMax(0),
      isExact(false)
    {}

    ArgSpec(int mi, int ma = -1, bool ex = false)
      :
      nargMin(mi),
      nargMax(ma == -1 ? mi : ma),
      isExact(ex)
    {}

    ArgSpec& min(int mi) { nargMin = mi; return *this; }
    ArgSpec& max(int ma) { nargMax = ma; return *this; }
    ArgSpec& exact(bool e) { isExact = e; return *this; }

    ArgSpec& nolimit()
    {
      nargMax = std::numeric_limits<unsigned int>::max();
      isExact = false;
      return *this;
    }

    unsigned int nargMin;
    unsigned int nargMax;
    bool         isExact;
  };
}

static const char vcid_groovx_tcl_argspec_h_utc20050628064704[] = "$Id$ $URL$";
#endif // !GROOVX_TCL_ARGSPEC_H_UTC20050628064704DEFINED
