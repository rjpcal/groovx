///////////////////////////////////////////////////////////////////////
//
// system.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Nov 17 15:05:41 1999
// written: Mon Mar  6 19:44:51 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SYSTEM_CC_DEFINED
#define SYSTEM_CC_DEFINED

#include "system.h"

#include "util/arrays.h"

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "trace.h"
#include "debug.h"

namespace {
  System* theSingleton = 0;
}

System::System () {
DOTRACE("System::System ");
}

System& System::theSystem() {
DOTRACE("System::theSystem");
  if (theSingleton == 0) {
	 theSingleton = new System;
  }
  return *theSingleton;
}

System::~System () {
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

int System::chmod(const char* path, mode_t mode) {
DOTRACE("System::chmod");
  return ::chmod(path, mode);
}

int System::rename(const char* oldpath, const char* newpath) {
DOTRACE("System::rename");
  return ::rename(oldpath, newpath);
}

int System::remove(const char* pathname) {
DOTRACE("System::remove");
  return ::remove(pathname);
}

const char* System::getcwd() {
DOTRACE("System::getcwd");
  const int INIT_SIZE = 256;
  static dynamic_block<char> buf(INIT_SIZE);

  while ( !::getcwd(&buf[0], buf.size()) ) {
	 buf.resize(buf.size() * 2);
  }

  return &buf[0];
}

static const char vcid_system_cc[] = "$Header$";
#endif // !SYSTEM_CC_DEFINED
