///////////////////////////////////////////////////////////////////////
// iomgr.h
// Rob Peters
// created: Fri Apr 23 01:12:37 1999
// written: Thu Mar 30 09:54:01 2000
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_H_DEFINED
#define IOMGR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

class istream;

namespace IO {
  namespace IoMgr {
	 IO::IoObject* newIO(const char* type);
	 IO::IoObject* newIO(istream &is, IO::IOFlag flag);
	 IO::IoObject* newIO(const char* type, istream &is, IO::IOFlag flag);
  }
}

static const char vcid_iomgr_h[] = "$Header$";
#endif // !IOMGR_H_DEFINED
