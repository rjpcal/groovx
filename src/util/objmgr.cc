///////////////////////////////////////////////////////////////////////
//
// iomgr.cc
// Rob Peters
// created: Fri Apr 23 01:13:16 1999
// written: Fri Sep 29 10:32:52 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_CC_DEFINED
#define IOMGR_CC_DEFINED

#include "io/iomgr.h"

#include "io/iofactory.h"

#define NO_TRACE
#include "util/trace.h"

IO::IoObject* IO::IoMgr::newIO(const char* type) {
DOTRACE("IO::IoMgr::newIO(const char*)");
  return IO::IoFactory::theOne().newCheckedObject(type);
}

static const char vcid_iomgr_cc[] = "$Header$";
#endif // !IOMGR_CC_DEFINED
