///////////////////////////////////////////////////////////////////////
//
// itertcl.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 17 14:02:32 2001
// written: Wed Sep 25 18:56:43 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ITERTCL_H_DEFINED
#define ITERTCL_H_DEFINED

#include "util/iter.h"

#include "tcl/tclconvert.h"
#include "tcl/tcllistobj.h"

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
