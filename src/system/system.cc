///////////////////////////////////////////////////////////////////////
//
// system.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 17 15:05:41 1999
// written: Mon Jan 13 11:04:47 2003
// $Id$
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
