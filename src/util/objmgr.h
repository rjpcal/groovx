///////////////////////////////////////////////////////////////////////
//
// objmgr.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Apr 23 01:12:37 1999
// written: Wed Aug  8 20:16:37 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJMGR_H_DEFINED
#define OBJMGR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REF_H_DEFINED)
#include "util/ref.h"
#endif

class fstring;

namespace Util {
  namespace ObjMgr {
    /// Will not return 0; will throw an exception on failure.
    Util::WeakRef<Util::Object> newObj(const char* type);

    /// Will not return 0; will throw an exception on failure.
    Util::WeakRef<Util::Object> newObj(const fstring& type);

    template <class T, class S>
    Util::WeakRef<T> newTypedObj(S type)
    {
      return dynamicCast<T>(newObj(type));
    }
  }
}

static const char vcid_objmgr_h[] = "$Header$";
#endif // !OBJMGR_H_DEFINED
