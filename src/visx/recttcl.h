///////////////////////////////////////////////////////////////////////
//
// recttcl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Aug  9 17:24:29 2001
// written: Tue Aug 28 09:56:03 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RECTTCL_H_DEFINED
#define RECTTCL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(RECT_H_DEFINED)
#include "gfx/rect.h"
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
  struct Convert<const Gfx::Rect<T>& >
  {
    typedef T Type;
    static Gfx::Rect<T> fromTcl( Tcl_Obj* obj )
    {
      Tcl::List listObj(obj);
      return Gfx::RectLTRB<T>(listObj.get(0, (T*)0), listObj.get(1, (T*)0),
                              listObj.get(2, (T*)0), listObj.get(3, (T*)0));
    }

    static Tcl::ObjPtr toTcl( const Gfx::Rect<T>& rect )
    {
      Tcl::List listObj;
      listObj.append(rect.left());
      listObj.append(rect.top());
      listObj.append(rect.right());
      listObj.append(rect.bottom());
      return listObj.asObj();
    }
  };

  template <class T>
  struct Convert<Gfx::Rect<T> >
  {
    typedef T Type;
    static Gfx::Rect<T> fromTcl( Tcl_Obj* obj )
    {
      return Convert<const Gfx::Rect<T>&>::fromTcl(obj);
    }

    static Tcl::ObjPtr toTcl( Gfx::Rect<T> rect )
    {
      return Convert<const Gfx::Rect<T>&>::toTcl(rect);
    }
  };
}

static const char vcid_recttcl_h[] = "$Header$";
#endif // !RECTTCL_H_DEFINED
