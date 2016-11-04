/** @file rutz/mutex.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Sep 13 09:15:02 2005
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

#ifndef GROOVX_RUTZ_MUTEX_H_UTC20050913161502_DEFINED
#define GROOVX_RUTZ_MUTEX_H_UTC20050913161502_DEFINED

#include <mutex>

#define GVX_MUTEX_CONCAT2(x,y) x##y
#define GVX_MUTEX_CONCAT(x,y) GVX_MUTEX_CONCAT2(x,y)

#define GVX_MUTEX_LOCK(mut) \
  std::lock_guard<decltype(mut)> GVX_MUTEX_CONCAT(anonymous_lock,__LINE__) (mut)

#endif // !GROOVX_RUTZ_MUTEX_H_UTC20050913161502DEFINED
