///////////////////////////////////////////////////////////////////////
// iomgr.h
// Rob Peters
// created: Fri Apr 23 01:12:37 1999
// written: Sat Sep 23 15:37:51 2000
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_H_DEFINED
#define IOMGR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#ifdef PRESTANDARD_IOSTREAMS
class istream;
class ostream;
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#endif

namespace IO {
  namespace IoMgr {
	 IO::IoObject* newIO(const char* type);
	 IO::IoObject* newIO(STD_IO::istream &is, IO::IOFlag flag);
	 IO::IoObject* newIO(const char* type, STD_IO::istream &is, IO::IOFlag flag);
  }
}

static const char vcid_iomgr_h[] = "$Header$";
#endif // !IOMGR_H_DEFINED
