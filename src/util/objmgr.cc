///////////////////////////////////////////////////////////////////////
//
// iomgr.cc
// Rob Peters
// created: Fri Apr 23 01:13:16 1999
// written: Wed Jun 30 17:17:19 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_CC_DEFINED
#define IOMGR_CC_DEFINED

#include "iomgr.h"

#include <iostream.h>
#include <typeinfo>
#include <string>

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

IO* IoMgr::newIO(const char* type) {
DOTRACE("IoMgr::newIO(const char*)");
  return IoFactory::theOne().newCheckedObject(type);
}

IO* IoMgr::newIO(istream& is, IO::IOFlag flag) {
DOTRACE("IoMgr::newIo(istream&, IO::IOFlag)");
  string type;
  is >> type;
  // We must turn off the TYPENAME flag since the typename has already
  // been read from the input stream by the present function.
  return newIO(type.c_str(), is, flag & ~IO::TYPENAME);
}

IO* IoMgr::newIO(const char* type, istream& is, IO::IOFlag flag) {
DOTRACE("IoMgr::newIo(const char*, istream&, IO::IOFlag)");
  IO* io = newIO(type);
  io->deserialize(is, flag);
  return io;
}

static const char vcid_iomgr_cc[] = "$Header$";
#endif // !IOMGR_CC_DEFINED
