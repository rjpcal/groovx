///////////////////////////////////////////////////////////////////////
//
// convert.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 08:57:31 2001
// written: Mon Jul 16 07:55:30 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CONVERT_H_DEFINED
#define CONVERT_H_DEFINED

struct Tcl_Obj;

namespace Tcl
{

  template <class T>
  struct Return
  {
    typedef T Type;
  };

  template <class T>
  struct Return<const T&>
  {
    typedef T Type;
  };

  template <class T>
  struct Convert
  {
    static Return<T>::Type   fromTcl( Tcl_Obj* obj );
    static Tcl_Obj*            toTcl( T val );
  };

}

static const char vcid_convert_h[] = "$Header$";
#endif // !CONVERT_H_DEFINED
