///////////////////////////////////////////////////////////////////////
//
// recttcl.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 14 18:59:55 2002
// written: Thu Nov 14 19:00:14 2002
// $Id$
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
