///////////////////////////////////////////////////////////////////////
// iomgr.h
// Rob Peters
// created: Fri Apr 23 01:12:37 1999
// written: Wed Jun 30 16:28:25 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_H_DEFINED
#define IOMGR_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef IOFACTORY_H_DEFINED
#include "iofactory.h"
#endif

class istream;

namespace IoMgr {
  IO* newIO(const char* type);
  IO* newIO(istream &is, IO::IOFlag flag);
  IO* newIO(const char* type, istream &is, IO::IOFlag flag);
}

static const char vcid_iomgr_h[] = "$Header$";
#endif // !IOMGR_H_DEFINED
