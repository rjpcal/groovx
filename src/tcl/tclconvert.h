///////////////////////////////////////////////////////////////////////
//
// tclconvert.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 08:57:31 2001
// written: Tue May 14 19:23:58 2002
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
  class ObjPtr;

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

  ///////////////////////////////////////////////////////////////////////
  /**
   *
   * This struct template is the foundation for the interface between
   * C++ and Tcl. Tcl::Convert has two functions, fromTcl() and
   * toTcl(), which translate C++ objects from and to Tcl objects (in
   * the form of Tcl_Obj*'s). Specializations for the basic numeric
   * and string types are provided in the implementation file, along
   * with specializations for Tcl types such as Tcl::List. Clients can
   * also provide their own specializations for other types. In
   * particular, Tcl::MemFunctor's convert Util::Object's to and from
   * Tcl by using their Util::UID's as handles.
   *
   **/
  ///////////////////////////////////////////////////////////////////////

  template <class T>
  struct Convert
  {
    /// Convert the Tcl object to C++.
    static typename Return<T>::Type   fromTcl( Tcl_Obj* obj );

    /// Convert the C++ object to Tcl.
    static Tcl::ObjPtr                  toTcl( T val );
  };

  template <>
  Tcl_Obj*  Convert<Tcl_Obj*>::fromTcl( Tcl_Obj* obj );

  template <>
  Tcl::ObjPtr Convert<Tcl_Obj*>::toTcl( Tcl_Obj* val );
}

static const char vcid_tclconvert_h[] = "$Header$";
#endif // !TCLCONVERT_H_DEFINED
