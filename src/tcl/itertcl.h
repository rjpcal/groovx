///////////////////////////////////////////////////////////////////////
//
// itertcl.h
//
// Copyright (c) 2001-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Aug 17 14:02:32 2001
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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
