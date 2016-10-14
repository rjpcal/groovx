/** @file rutz/spin_lock_pthreads.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Sep  5 12:21:04 2007
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

#ifndef GROOVX_RUTZ_SPIN_LOCK_PTHREADS_CC_UTC20070905192104_DEFINED
#define GROOVX_RUTZ_SPIN_LOCK_PTHREADS_CC_UTC20070905192104_DEFINED

#ifdef HAVE_PTHREAD_SPINLOCK_T

#include "rutz/spin_lock_pthreads.h"

#include "rutz/sfmt.h"

#include <string.h>

rutz::spin_lock_pthreads::spin_lock_pthreads()
{
  const int c = pthread_spin_init(&m_lock, 0);
  if (c != 0)
    {
      char buf[256];
      strerror_r(c, &buf[0], sizeof(buf));
      buf[sizeof(buf)-1] = '\0';
      throw rutz::error(rutz::sfmt("pthread_spin_init() failed (%s)",
                                   &buf[0]),
                        SRC_POS);
    }
}

rutz::spin_lock_pthreads::~spin_lock_pthreads()
{
  pthread_spin_destroy(&m_lock);
}

#endif // HAVE_PTHREAD_SPINLOCK_T

#endif // !GROOVX_RUTZ_SPIN_LOCK_PTHREADS_CC_UTC20070905192104DEFINED
