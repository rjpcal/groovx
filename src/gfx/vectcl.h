///////////////////////////////////////////////////////////////////////
//
// vectcl.h
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Nov 20 14:06:16 2004
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

#ifndef VECTCL_H_DEFINED
#define VECTCL_H_DEFINED

#include "geom/vec2.h"
#include "geom/vec3.h"

#include "tcl/tclconvert.h"
#include "tcl/tcllistobj.h"

#include "util/error.h"

namespace Tcl
{
  /// Conversion routine for Tcl object to geom::vec2.
  template <class T>
  inline geom::vec2<T> fromTcl(Tcl_Obj* obj, geom::vec2<T>*)
  {
    Tcl::List listObj(obj);

    return geom::vec2<T>(listObj.template get<T>(0),
                         listObj.template get<T>(1));
  }

  /// Conversion routine for geom::vec2 to Tcl::ObjPtr.
  template <class T>
  inline Tcl::ObjPtr toTcl( const geom::vec2<T>& v )
  {
    Tcl::List listObj;
    listObj.append(v.x());
    listObj.append(v.y());
    return listObj.asObj();
  }

  /// Conversion routine for Tcl object to geom::vec3.
  template <class T>
  inline geom::vec3<T> fromTcl(Tcl_Obj* obj, geom::vec3<T>*)
  {
    Tcl::List listObj(obj);

    return geom::vec3<T>(listObj.template get<T>(0),
                         listObj.template get<T>(1),
                         listObj.template get<T>(2));
  }

  /// Conversion routine for geom::vec3 to Tcl::ObjPtr.
  template <class T>
  inline Tcl::ObjPtr toTcl( const geom::vec3<T>& v )
  {
    Tcl::List listObj;
    listObj.append(v.x());
    listObj.append(v.y());
    listObj.append(v.z());
    return listObj.asObj();
  }
}

static const char vcid_vectcl_h[] = "$Header$";
#endif // !VECTCL_H_DEFINED
