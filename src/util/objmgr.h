///////////////////////////////////////////////////////////////////////
//
// iomgr.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Apr 23 01:12:37 1999
// written: Thu May 17 06:57:15 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_H_DEFINED
#define IOMGR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IODECLS_H_DEFINED)
#include "io/iodecls.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRHANDLE_H_DEFINED)
#include "util/ptrhandle.h"
#endif

class fixed_string;

namespace IO {
  namespace IoMgr {
	 /// Will not return 0; will throw an exception on failure.
	 IO::IoObject* newIO(const char* type);

	 /// Will not return 0; will throw an exception on failure.
	 IO::IoObject* newIO(const fixed_string& type);

	 template <class T, class S>
	 PtrHandle<T> newTypedIO(S type)
	 {
		PtrHandle<IO::IoObject> io(newIO(type));
  		return dynamicCast<T>(io);
	 }
  }
}

static const char vcid_iomgr_h[] = "$Header$";
#endif // !IOMGR_H_DEFINED
