///////////////////////////////////////////////////////////////////////
//
// randutils.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Mar  8 02:35:53 1999
// written: Wed Mar 19 17:58:52 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef RANDUTILS_H_DEFINED
#define RANDUTILS_H_DEFINED

#include <cstdlib>

namespace Util
{
  template <class T>
  inline T randRange(const T& min, const T& max)
  {
    return T( (double(rand()) / (double(RAND_MAX)+1.0)) * (max-min) + min );
  }
}

static const char vcid_randutils_h[] = "$Header$";
#endif // !RANDUTILS_H_DEFINED
