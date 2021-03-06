/** @file rutz/spin_lock_darwin.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Sep  4 14:08:02 2007
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_RUTZ_SPIN_LOCK_DARWIN_H_UTC20070904210802_DEFINED
#define GROOVX_RUTZ_SPIN_LOCK_DARWIN_H_UTC20070904210802_DEFINED

#include <libkern/OSAtomic.h>

namespace rutz
{

class spin_lock_darwin
{
private:
  OSSpinLock m_lock;

  spin_lock_darwin(const spin_lock_darwin&); // not implemented
  spin_lock_darwin& operator=(const spin_lock_darwin&); // not implemented

public:
  spin_lock_darwin()
  { OSSpinLockUnlock(&m_lock); }

  void lock()
  { OSSpinLockLock(&m_lock); }

  bool try_lock()
  { return OSSpinLockTry(&m_lock); }

  void unlock()
  { OSSpinLockUnlock(&m_lock); }
};

}

#endif // !GROOVX_RUTZ_SPIN_LOCK_DARWIN_H_UTC20070904210802DEFINED
