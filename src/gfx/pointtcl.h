///////////////////////////////////////////////////////////////////////
//
// pointtcl.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Aug  9 17:47:29 2001
// commit: $Id$
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

#ifndef POINTTCL_H_DEFINED
#define POINTTCL_H_DEFINED

#include "geom/vec2.h"

#include "tcl/tclconvert.h"
#include "tcl/tcllistobj.h"

namespace Tcl
{
  /// Conversion routine for Tcl object to geom::vec2.
  template <class T>
  inline geom::vec2<T> fromTcl(Tcl_Obj* obj, geom::vec2<T>*)
  {
    Tcl::List listObj(obj);
    return geom::vec2<T>(listObj.get(0, (T*)0), listObj.get(1, (T*)0));
  }

  /// Conversion routine for geom::vec2 to Tcl::ObjPtr.
  template <class T>
  inline Tcl::ObjPtr toTcl( const geom::vec2<T>& point )
  {
    Tcl::List listObj;
    listObj.append(point.x());
    listObj.append(point.y());
    return listObj.asObj();
  }
}

static const char vcid_pointtcl_h[] = "$Header$";
#endif // !POINTTCL_H_DEFINED
