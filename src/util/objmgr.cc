///////////////////////////////////////////////////////////////////////
//
// iomgr.cc
// Rob Peters
// created: Fri Apr 23 01:13:16 1999
// written: Tue Sep 26 18:25:31 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_CC_DEFINED
#define IOMGR_CC_DEFINED

#include "io/iomgr.h"

#include "io/iofactory.h"

#include "util/strings.h"

#include <iostream.h>
#include <typeinfo>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

IO::IoObject* IO::IoMgr::newIO(const char* type) {
DOTRACE("IO::IoMgr::newIO(const char*)");
  return IO::IoFactory::theOne().newCheckedObject(type);
}

IO::IoObject* IO::IoMgr::newIO(STD_IO::istream& is, IO::IOFlag flag) {
DOTRACE("IO::IoMgr::newIo(STD_IO::istream&, IO::IOFlag)");
  fixed_string type;
  is >> type;
  // We must turn off the IO::TYPENAME flag since the typename has already
  // been read from the input stream by the present function.
  return newIO(type.c_str(), is, flag & ~IO::TYPENAME);
}

IO::IoObject* IO::IoMgr::newIO(const char* type, STD_IO::istream& is, IO::IOFlag flag) {
DOTRACE("IO::IoMgr::newIo(const char*, STD_IO::istream&, IO::IOFlag)");
  IO::IoObject* io = newIO(type);
  io->ioDeserialize(is, flag);
  return io;
}

static const char vcid_iomgr_cc[] = "$Header$";
#endif // !IOMGR_CC_DEFINED
