///////////////////////////////////////////////////////////////////////
// iomgr.h
// Rob Peters
// created: Fri Apr 23 01:12:37 1999
// written: Wed Mar 22 16:47:04 2000
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_H_DEFINED
#define IOMGR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H)
#include "io.h"
#endif

class istream;

namespace IoMgr {
  IO* newIO(const char* type);
  IO* newIO(istream &is, IO::IOFlag flag);
  IO* newIO(const char* type, istream &is, IO::IOFlag flag);
}

static const char vcid_iomgr_h[] = "$Header$";
#endif // !IOMGR_H_DEFINED
