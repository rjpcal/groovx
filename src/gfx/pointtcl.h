///////////////////////////////////////////////////////////////////////
//
// pointtcl.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Aug  9 17:47:29 2001
// written: Mon Jan 13 11:01:38 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POINTTCL_H_DEFINED
#define POINTTCL_H_DEFINED

#include "gx/vec2.h"

#include "tcl/tclconvert.h"
#include "tcl/tcllistobj.h"

namespace Tcl
{
  /// Conversion routine for Tcl object to Gfx::Vec2.
  template <class T>
  inline Gfx::Vec2<T> fromTcl(Tcl_Obj* obj, Gfx::Vec2<T>*)
  {
    Tcl::List listObj(obj);
    return Gfx::Vec2<T>(listObj.get(0, (T*)0), listObj.get(1, (T*)0));
  }

  /// Conversion routine for Gfx::Vec2 to Tcl::ObjPtr.
  template <class T>
  inline Tcl::ObjPtr toTcl( const Gfx::Vec2<T>& point )
  {
    Tcl::List listObj;
    listObj.append(point.x());
    listObj.append(point.y());
    return listObj.asObj();
  }
}

static const char vcid_pointtcl_h[] = "$Header$";
#endif // !POINTTCL_H_DEFINED
