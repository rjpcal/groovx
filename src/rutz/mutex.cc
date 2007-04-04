/** @file rutz/mutex.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2007
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Mar 30 10:25:47 2007
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

#ifndef GROOVX_RUTZ_MUTEX_CC_UTC20070330172547_DEFINED
#define GROOVX_RUTZ_MUTEX_CC_UTC20070330172547_DEFINED

#include "rutz/mutex.h"

#include "rutz/error.h"

rutz::mutex_lock_class::mutex_lock_class(pthread_mutex_t* mut)
  : m_mutex(mut)
{
  if (m_mutex)
    {
      if (pthread_mutex_lock(m_mutex) != 0)
        throw rutz::error("pthread_mutex_lock failed", SRC_POS);
    }
}

void rutz::mutex_lock_class::unlock() throw()
{
  if (m_mutex)
    {
      pthread_mutex_unlock(m_mutex);
      m_mutex = 0;
    }
}

static const char vcid_groovx_rutz_mutex_cc_utc20070330172547[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_MUTEX_CC_UTC20070330172547DEFINED
