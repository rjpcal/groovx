///////////////////////////////////////////////////////////////////////
//
// tclobjptr.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 11 13:44:19 1999
// written: Wed Sep 25 18:57:09 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLOBJPTR_H_DEFINED
#define TCLOBJPTR_H_DEFINED

#include "tcl/tclconvert.h"

struct Tcl_Obj;

namespace Tcl
{

///////////////////////////////////////////////////////////////////////
/**
 *
 * This class acts as a reference counted smart pointer to Tcl_Obj. It
 * is similar to TclObjLock in that it manages the reference count of
 * a held Tcl_Obj*, but it also defines a conversion operator back to
 * Tcl_Obj* in order to provide access to Tcl_Obj's interface.
 *
 **/
///////////////////////////////////////////////////////////////////////

class ObjPtr
{
public:
  /// Default constructor with a shared and empty Tcl_Obj*.
  ObjPtr();

  /// Construct with a Tcl_Obj*.
  ObjPtr(Tcl_Obj* obj) : itsObj(obj) { incrRef(itsObj); }

  /// Construct with automatic conversion from a C++ type.
  template <class T>
  ObjPtr(T t) : itsObj(0)
  {
    ObjPtr temp(Tcl::Convert<T>::toTcl(t));
    itsObj = temp.obj();
    incrRef(itsObj);
  }

  /// Destructor.
  ~ObjPtr() { decrRef(itsObj); }

  /// Copy constructor.
  ObjPtr(const ObjPtr& x) :
    itsObj(x.itsObj)
    {
      incrRef(itsObj);
    }

  /// Assignment operator from ObjPtr.
  ObjPtr& operator=(const ObjPtr& x)
    {
      assign(x.itsObj); return *this;
    }

  /// Assignment operator from Tcl_Obj*.
  ObjPtr& operator=(Tcl_Obj* x)
    {
      assign(x); return *this;
    }

  typedef Tcl_Obj* Tcl_ObjPtr;

  Tcl_Obj* obj() const { return itsObj; }

  // FIXME: really need this function?
  template <class Cue>
  typename Cue::Type as(Cue) const
  {
    return Tcl::Convert<typename Cue::Type>::fromTcl(itsObj);
  }

  template <class T>
  T as() const
  {
    return Tcl::Convert<T>::fromTcl(itsObj);
  }

  void append(const Tcl::ObjPtr& other);

  bool isShared() const;
  bool isUnique() const { return !isShared(); }

  void ensureUnique() const;

  const char* typeName() const;

private:
  static void incrRef(Tcl_Obj* obj);
  static void decrRef(Tcl_Obj* obj);

  void assign(Tcl_Obj* x) const
    {
      if (itsObj != x)
        {
          decrRef(itsObj);
          itsObj = x;
          incrRef(itsObj);
        }
    }

  mutable Tcl_Obj* itsObj;
};

} // end namespace Tcl

static const char vcid_tclobjptr_h[] = "$Header$";
#endif // !TCLOBJPTR_H_DEFINED
