///////////////////////////////////////////////////////////////////////
//
// pointtcl.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Aug  9 17:47:29 2001
// written: Wed Sep 25 19:02:54 2002
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
  /// Conversion routines for Gfx::Vec2 to and from Tcl::ObjPtr.
  template <class T>
  struct Convert<const Gfx::Vec2<T>& >
  {
    typedef T Type;
    static Gfx::Vec2<T> fromTcl( Tcl_Obj* obj )
    {
      Tcl::List listObj(obj);
      return Gfx::Vec2<T>(listObj.get(0, (T*)0), listObj.get(1, (T*)0));
    }

    static Tcl::ObjPtr toTcl( const Gfx::Vec2<T>& point )
    {
      Tcl::List listObj;
      listObj.append(point.x());
      listObj.append(point.y());
      return listObj.asObj();
    }
  };

  /// Conversion routines for Gfx::Vec2 to and from Tcl::ObjPtr.
  template <class T>
  struct Convert<Gfx::Vec2<T> >
  {
    typedef T Type;
    static Gfx::Vec2<T> fromTcl( Tcl_Obj* obj )
    {
      return Convert<const Gfx::Vec2<T>&>::fromTcl(obj);
    }

    static Tcl::ObjPtr toTcl( Gfx::Vec2<T> point )
    {
      return Convert<const Gfx::Vec2<T>&>::toTcl(point);
    }
  };
}

static const char vcid_pointtcl_h[] = "$Header$";
#endif // !POINTTCL_H_DEFINED
