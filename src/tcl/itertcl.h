///////////////////////////////////////////////////////////////////////
//
// itertcl.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 17 14:02:32 2001
// written: Sun Nov  3 09:10:49 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ITERTCL_H_DEFINED
#define ITERTCL_H_DEFINED

#include "util/iter.h"

#include "tcl/tcllistobj.h"

namespace Tcl
{
  template <class T>
  inline Util::FwdIter<T> fromTcl(Tcl_Obj* obj, Util::FwdIter<T>*)
  {
    Tcl::List l(obj);
    return Util::FwdIter<T>(l.template begin<T>(), l.template end<T>());
  }

  template <class T>
  inline Tcl::ObjPtr toTcl( Util::FwdIter<T> iter )
  {
    Tcl::List result;
    while ( !iter.atEnd() )
      {
        result.append(*iter);
        iter.next();
      }
    return result.asObj();
  }
}

static const char vcid_itertcl_h[] = "$Header$";
#endif // !ITERTCL_H_DEFINED
