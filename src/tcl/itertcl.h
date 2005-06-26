///////////////////////////////////////////////////////////////////////
//
// itertcl.h
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Aug 17 14:02:32 2001
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef ITERTCL_H_DEFINED
#define ITERTCL_H_DEFINED

#include "rutz/iter.h"

#include "tcl/tcllistobj.h"

namespace Tcl
{
  template <class T>
  inline rutz::fwd_iter<T> fromTcl(Tcl_Obj* obj, rutz::fwd_iter<T>*)
  {
    Tcl::List l(obj);
    return rutz::fwd_iter<T>(l.template begin<T>(), l.template end<T>());
  }

  template <class T>
  inline Tcl::ObjPtr toTcl( rutz::fwd_iter<T> iter )
  {
    Tcl::List result;
    while ( !iter.at_end() )
      {
        result.append(*iter);
        iter.next();
      }
    return result.asObj();
  }
}

static const char vcid_itertcl_h[] = "$Id$ $URL$";
#endif // !ITERTCL_H_DEFINED
