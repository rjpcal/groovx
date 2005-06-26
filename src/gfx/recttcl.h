///////////////////////////////////////////////////////////////////////
//
// recttcl.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Nov 14 18:59:55 2002
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

#ifndef RECTTCL_H_DEFINED
#define RECTTCL_H_DEFINED

#include "geom/rect.h"

#include "tcl/tclconvert.h"
#include "tcl/tcllistobj.h"

#include "rutz/error.h"

namespace Tcl
{
  /// Conversion routines for Tcl object to geom::rect.
  template <class T>
  inline geom::rect<T> fromTcl(Tcl_Obj* obj, geom::rect<T>*)
  {
    Tcl::List listObj(obj);
    geom::rect<T> result = geom::rect<T>::ltrb(listObj.template get<T>(0),
                                               listObj.template get<T>(1),
                                               listObj.template get<T>(2),
                                               listObj.template get<T>(3));

    if (result.width() <= 0.0)
      throw rutz::error("invalid rect (width was <= 0)", SRC_POS);

    if (result.height() <= 0.0)
      throw rutz::error("invalid rect (height was <= 0)", SRC_POS);

    return result;
  }

  /// Conversion routine for geom::rect to Tcl::ObjPtr.
  template <class T>
  inline Tcl::ObjPtr toTcl( const geom::rect<T>& rect )
  {
    Tcl::List listObj;
    listObj.append(rect.left());
    listObj.append(rect.top());
    listObj.append(rect.right());
    listObj.append(rect.bottom());
    return listObj.asObj();
  }
}

static const char vcid_recttcl_h[] = "$Id$ $URL$";
#endif // !RECTTCL_H_DEFINED
