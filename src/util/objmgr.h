///////////////////////////////////////////////////////////////////////
//
// objmgr.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Apr 23 01:12:37 1999
// written: Wed Sep 25 18:59:27 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJMGR_H_DEFINED
#define OBJMGR_H_DEFINED

#include "util/ref.h"

class fstring;

namespace Util
{
  namespace ObjMgr
  {
    /// Will not return 0; will throw an exception on failure.
    Util::SoftRef<Util::Object> newObj(const char* type);

    /// Will not return 0; will throw an exception on failure.
    Util::SoftRef<Util::Object> newObj(const fstring& type);

    template <class T, class S>
    Util::SoftRef<T> newTypedObj(S type)
    {
      return dynamicCast<T>(newObj(type));
    }
  }
}

static const char vcid_objmgr_h[] = "$Header$";
#endif // !OBJMGR_H_DEFINED
