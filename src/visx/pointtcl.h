///////////////////////////////////////////////////////////////////////
//
// pointtcl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Aug  9 17:47:29 2001
// written: Thu Aug  9 18:41:21 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POINTTCL_H_DEFINED
#define POINTTCL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINT_H_DEFINED)
#include "point.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCONVERT_H_DEFINED)
#include "tcl/tclconvert.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLLISTOBJ_H_DEFINED)
#include "tcl/tcllistobj.h"
#endif

namespace Tcl
{
  template <class T>
  struct Convert<const Point<T>& >
  {
    typedef T Type;
    static Point<T> fromTcl( Tcl_Obj* obj )
    {
      Tcl::List listObj(obj);
      return Point<T>(listObj.get(0, (T*)0), listObj.get(1, (T*)0));
    }

    static Tcl::ObjPtr toTcl( const Point<T>& point )
    {
      Tcl::List listObj;
      listObj.append(point.x());
      listObj.append(point.y());
      return listObj.asObj();
    }
  };

  template <class T>
  struct Convert<Point<T> >
  {
    typedef T Type;
    static Point<T> fromTcl( Tcl_Obj* obj )
    {
      return Convert<const Point<T>&>::fromTcl(obj);
    }

    static Tcl::ObjPtr toTcl( Point<T> point )
    {
      return Convert<const Point<T>&>::toTcl(point);
    }
  };
}

static const char vcid_pointtcl_h[] = "$Header$";
#endif // !POINTTCL_H_DEFINED
