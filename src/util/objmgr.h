///////////////////////////////////////////////////////////////////////
// iomgr.h
// Rob Peters
// created: Fri Apr 23 01:12:37 1999
// written: Fri Nov  3 14:44:05 2000
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_H_DEFINED
#define IOMGR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IODECLS_H_DEFINED)
#include "io/iodecls.h"
#endif

class fixed_string;

namespace IO {
  namespace IoMgr {
	 IO::IoObject* newIO(const char* type);
	 IO::IoObject* newIO(const fixed_string& type);
  }
}

static const char vcid_iomgr_h[] = "$Header$";
#endif // !IOMGR_H_DEFINED
