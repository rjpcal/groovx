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
  /// Specify how many args a command can take.
  /** By convention, nargMin and nargMax INCLUDE the zero'th argument
      (i.e. the command name) in the arg count. Thus a command that
      takes no parameters would have an arg count of 1. If isExact is
      true, then the argc of a command invocation is required to be
      exactly equal either nargMin or nargMax; if it is false, then
      argc must be between nargMin and nargMax, inclusive. */
  struct ArgSpec
  {
    ArgSpec()
      :
      nargMin(0),
      nargMax(0),
      isExact(false)
    {}

    /// Construct with initial values for nargMin/nargMax/isExact.
    /** If the value given for nmax is negative, then nargMax will be
        set to the same value as nmin. */
    explicit ArgSpec(int nmin, int nmax = -1, bool ex = false)
      :
      nargMin(nmin < 0
              ? 0
              : static_cast<unsigned int>(nmin)),
      nargMax(nmax == -1
              ? nargMin
              : static_cast<unsigned int>(nmax)),
      isExact(ex)
    {}

    ArgSpec& min(int nmin) { nargMin = nmin; return *this; }
    ArgSpec& max(int nmax) { nargMax = nmax; return *this; }
    ArgSpec& exact(bool ex) { isExact = ex; return *this; }

    ArgSpec& nolimit()
    {
      nargMax = std::numeric_limits<unsigned int>::max();
      isExact = false;
      return *this;
    }

    bool allowsObjc(unsigned int objc) const
    {
      if (this->isExact)
        {
          return (objc == this->nargMin ||
                  objc == this->nargMax);
        }
      // else...
      return (objc >= this->nargMin &&
              objc <= this->nargMax);
    }

    unsigned int nargMin;
    unsigned int nargMax;
    bool         isExact;
  };
}

static const char vcid_groovx_tcl_argspec_h_utc20050628064704[] = "$Id$ $URL$";
#endif // !GROOVX_TCL_ARGSPEC_H_UTC20050628064704DEFINED
