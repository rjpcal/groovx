///////////////////////////////////////////////////////////////////////
//
// system.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 17 15:05:41 1999
// commit: $Id$
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

#ifndef SYSTEM_CC_DEFINED
#define SYSTEM_CC_DEFINED

#include "unixcall.h"

#include "util/arrays.h"
#include "util/error.h"
#include "util/fstring.h"

#include <cerrno> // for ::errno
#include <cstdio> // for ::rename(), ::remove()
#include <cstring> // for ::strerror()
#include <sys/stat.h> // for ::chmod()
#include <unistd.h> // for ::getcwd() (POSIX)

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
  void throwErrno(const char* where, const rutz::file_pos& pos)
  {
    throw rutz::error(rutz::fstring("in \"", where, "\": ",
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
DOTRACE("rutz::unixcall::chmod");

  ErrnoSaver saver;

  if ( ::chmod(path, mode) != 0 )
    throwErrno("chmod", SRC_POS);
}

void rutz::unixcall::rename(const char* oldpath, const char* newpath)
{
DOTRACE("rutz::unixcall::rename");

  ErrnoSaver saver;

  if ( ::rename(oldpath, newpath) != 0 )
    throwErrno("rename", SRC_POS);
}

void rutz::unixcall::remove(const char* pathname)
{
DOTRACE("rutz::unixcall::remove");

  errno = 0;

  if ( ::remove(pathname) != 0 )
    throwErrno("rutz::unixcall::remove", SRC_POS);
}

rutz::fstring rutz::unixcall::getcwd()
{
DOTRACE("rutz::unixcall::getcwd");

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

static const char vcid_system_cc[] = "$Header$";
#endif // !SYSTEM_CC_DEFINED
