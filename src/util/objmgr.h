///////////////////////////////////////////////////////////////////////
// iomgr.h
// Rob Peters
// created: Fri Apr 23 01:12:37 1999
// written: Sat Sep 23 15:06:04 2000
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_H_DEFINED
#define IOMGR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#include <iosfwd>
#define IOSFWD_DEFINED
#endif

namespace IO {
  namespace IoMgr {
	 IO::IoObject* newIO(const char* type);
	 IO::IoObject* newIO(std::istream &is, IO::IOFlag flag);
	 IO::IoObject* newIO(const char* type, std::istream &is, IO::IOFlag flag);
  }
}

static const char vcid_iomgr_h[] = "$Header$";
#endif // !IOMGR_H_DEFINED
