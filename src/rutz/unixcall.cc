/** @file rutz/unixcall.cc thin wrappers around unix system calls */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 17 15:05:41 1999
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_RUTZ_UNIXCALL_CC_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_UNIXCALL_CC_UTC20050626084019_DEFINED

#include "unixcall.h"

#include "rutz/arrays.h"
#include "rutz/error.h"
#include "rutz/fstring.h"

#include <cerrno> // for ::errno
#include <cstdio> // for ::rename(), ::remove()
#include <cstring> // for ::strerror()
#include <sys/stat.h> // for ::chmod()
#include <unistd.h> // for ::getcwd() (POSIX)

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  void throwErrno(const char* where, const rutz::file_pos& pos)
  {
    throw rutz::error(rutz::cat("in \"", where, "\": ",
                                ::strerror(errno)), pos);
  }

  class ErrnoSaver
  {
  public:
    ErrnoSaver() : saved(errno)
    {
      errno = 0;
    }

    ~ErrnoSaver()
    {
      errno = saved;
    }

    const int saved;
  };
}

void rutz::unixcall::chmod(const char* path, mode_t mode)
{
GVX_TRACE("rutz::unixcall::chmod");

  ErrnoSaver saver;

  if ( ::chmod(path, mode) != 0 )
    throwErrno("chmod", SRC_POS);
}

void rutz::unixcall::rename(const char* oldpath, const char* newpath)
{
GVX_TRACE("rutz::unixcall::rename");

  ErrnoSaver saver;

  if ( ::rename(oldpath, newpath) != 0 )
    throwErrno("rename", SRC_POS);
}

void rutz::unixcall::remove(const char* pathname)
{
GVX_TRACE("rutz::unixcall::remove");

  errno = 0;

  if ( ::remove(pathname) != 0 )
    throwErrno("rutz::unixcall::remove", SRC_POS);
}

rutz::fstring rutz::unixcall::getcwd()
{
GVX_TRACE("rutz::unixcall::getcwd");

  ErrnoSaver saver;

  const int INIT_SIZE = 256;
  rutz::dynamic_block<char> buf(INIT_SIZE);

  while ( ::getcwd(&buf[0], buf.size()) == 0 )
    {
      if (errno == ERANGE)
        {
          errno = 0;
          buf.resize(buf.size() * 2);
        }
      else
        {
          throwErrno("getcwd", SRC_POS);
        }
    }

  return rutz::fstring(&buf[0]);
}

static const char vcid_groovx_rutz_unixcall_cc_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_UNIXCALL_CC_UTC20050626084019_DEFINED
