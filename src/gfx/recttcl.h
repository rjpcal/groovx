///////////////////////////////////////////////////////////////////////
//
// recttcl.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Nov 14 18:59:55 2002
// written: Wed Mar 19 17:55:58 2003
// $Id$
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

#ifndef RECTTCL_H_DEFINED
#define RECTTCL_H_DEFINED

#include "gx/rect.h"

#include "tcl/tclconvert.h"
#include "tcl/tcllistobj.h"

namespace Tcl
{
  /// Conversion routines for Tcl object to Gfx::Rect.
  template <class T>
  inline Gfx::Rect<T> fromTcl(Tcl_Obj* obj, Gfx::Rect<T>*)
  {
    Tcl::List listObj(obj);
    return Gfx::RectLTRB<T>(listObj.get(0, (T*)0), listObj.get(1, (T*)0),
                            listObj.get(2, (T*)0), listObj.get(3, (T*)0));
  }

  /// Conversion routine for Gfx::Rect to Tcl::ObjPtr.
  template <class T>
  inline Tcl::ObjPtr toTcl( const Gfx::Rect<T>& rect )
  {
    Tcl::List listObj;
    listObj.append(rect.left());
    listObj.append(rect.top());
    listObj.append(rect.right());
    listObj.append(rect.bottom());
    return listObj.asObj();
  }
}

static const char vcid_recttcl_h[] = "$Header$";
#endif // !RECTTCL_H_DEFINED
