///////////////////////////////////////////////////////////////////////
//
// itertcl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 17 14:02:32 2001
// written: Fri Aug 17 14:04:25 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ITERTCL_H_DEFINED
#define ITERTCL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ITER_H_DEFINED)
#include "util/iter.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCONVERT_H_DEFINED)
#include "tcl/tclconvert.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLLISTOBJ_H_DEFINED)
#include "tcl/tcllistobj.h"
#endif

namespace Tcl
{
  template <class T>
  struct Convert<Util::FwdIter<T> >
  {
    /// Convert the C++ object to Tcl.
    static Tcl::ObjPtr toTcl( Util::FwdIter<T> iter )
    {
      Tcl::List result;
      while ( !iter.atEnd() )
        {
          result.append(*iter);
          iter.next();
        }
      return result.asObj();
    }
  };
}

static const char vcid_itertcl_h[] = "$Header$";
#endif // !ITERTCL_H_DEFINED
