///////////////////////////////////////////////////////////////////////
//
// system.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
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

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  System* theSingleton = 0;

  void throwErrno(const char* where)
  {
    throw Util::Error(fstring("in \"", where, "\": ", ::strerror(errno)));
  }
}

System::System ()
{
DOTRACE("System::System ");
}

System& System::theSystem()
{
DOTRACE("System::theSystem");
  if (theSingleton == 0)
    {
      theSingleton = new System;
    }
  return *theSingleton;
}

System::~System ()
{
DOTRACE("System::~System ");
}

const System::mode_t System::IRUSR;
const System::mode_t System::IWUSR;
const System::mode_t System::IXUSR;
const System::mode_t System::IRGRP;
const System::mode_t System::IWGRP;
const System::mode_t System::IXGRP;
const System::mode_t System::IROTH;
const System::mode_t System::IWOTH;
const System::mode_t System::IXOTH;

void System::chmod(const char* path, mode_t mode)
{
DOTRACE("System::chmod");

  if ( ::chmod(path, mode) != 0 )
    throwErrno("System::chmod");
}

void System::rename(const char* oldpath, const char* newpath)
{
DOTRACE("System::rename");

  if ( ::rename(oldpath, newpath) != 0 )
    throwErrno("System::rename");
}

void System::remove(const char* pathname)
{
DOTRACE("System::remove");

  if ( ::remove(pathname) != 0 )
    throwErrno("System::remove");
}

const char* System::getcwd()
{
DOTRACE("System::getcwd");
  const int INIT_SIZE = 256;
  static dynamic_block<char> buf(INIT_SIZE);

  while ( !::getcwd(&buf[0], buf.size()) )
         {
                buf.resize(buf.size() * 2);
         }

  return &buf[0];
}

const char* System::getenv(const char* environment_variable)
{
DOTRACE("System::getenv");
  return ::getenv(environment_variable);
}

void System::sleep(unsigned int seconds)
{
DOTRACE("System::sleep");
  ::sleep(seconds);
}

fstring System::formattedTime(const char* format)
{
DOTRACE("System::formattedTime");

  time_t t = time(0);

  struct tm* tt = localtime(&t);

  char buf[512];

  std::size_t count = strftime(buf, 512, format, tt);

  return fstring(Util::CharData(&buf[0], count));
}

static const char vcid_system_cc[] = "$Header$";
#endif // !SYSTEM_CC_DEFINED
