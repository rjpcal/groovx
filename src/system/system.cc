///////////////////////////////////////////////////////////////////////
//
// system.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 17 15:05:41 1999
// commit: $Id$
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

#ifndef SYSTEM_CC_DEFINED
#define SYSTEM_CC_DEFINED

#include "system/system.h"

#include "util/arrays.h"
#include "util/error.h"
#include "util/strings.h"

#include <cerrno> // for ::errno
#include <cstdio> // for ::rename(), ::remove()
#include <cstring> // for ::strerror()
#include <sys/stat.h> // for ::chmod()

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
  void throwErrno(const char* where)
  {
    throw Util::Error(fstring("in \"", where, "\": ", ::strerror(errno)));
  }
}

void unixcall::chmod(const char* path, mode_t mode)
{
DOTRACE("unixcall::chmod");

  if ( ::chmod(path, mode) != 0 )
    throwErrno("unixcall::chmod");
}

void unixcall::rename(const char* oldpath, const char* newpath)
{
DOTRACE("unixcall::rename");

  if ( ::rename(oldpath, newpath) != 0 )
    throwErrno("unixcall::rename");
}

void unixcall::remove(const char* pathname)
{
DOTRACE("unixcall::remove");

  if ( ::remove(pathname) != 0 )
    throwErrno("unixcall::remove");
}

fstring unixcall::getcwd()
{
DOTRACE("unixcall::getcwd");
  const int INIT_SIZE = 256;
  dynamic_block<char> buf(INIT_SIZE);

  errno = 0;
  while ( ::getcwd(&buf[0], buf.size()) == 0 )
    {
      if (errno == ERANGE)
        {
          errno = 0;
          buf.resize(buf.size() * 2);
        }
      else
        {
          throwErrno("unixcall::getcwd");
        }
    }

  return fstring(&buf[0]);
}

static const char vcid_system_cc[] = "$Header$";
#endif // !SYSTEM_CC_DEFINED
