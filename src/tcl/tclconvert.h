///////////////////////////////////////////////////////////////////////
//
// tclconvert.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 08:57:31 2001
// written: Mon Jul 16 14:21:26 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCONVERT_H_DEFINED
#define TCLCONVERT_H_DEFINED

struct Tcl_Obj;

template <class T>
struct TypeCue
{
  typedef T Type;
};

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

static const char vcid_tclconvert_h[] = "$Header$";
#endif // !TCLCONVERT_H_DEFINED
