///////////////////////////////////////////////////////////////////////
//
// tclobjptr.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 11 13:44:19 1999
// written: Mon Jul 16 13:19:13 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLOBJPTR_H_DEFINED
#define TCLOBJPTR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CONVERT_H_DEFINED)
#include "tcl/convert.h"
#endif

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

class ObjPtr {
public:
  /// Default constructor with a shared and empty Tcl_Obj*.
  ObjPtr();

  /// Construct with automatic conversion from a C++ type.
  template <class T>
  ObjPtr(T t) : itsObj(Tcl::Convert<T>::toTcl(t)) { incrRef(itsObj); }

  /// Destructor.
  ~ObjPtr() { decrRef(itsObj); }

  /// Copy constructor.
  ObjPtr(const ObjPtr& x) :
    itsObj(const_cast<Tcl_Obj*>(x.itsObj))
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

  /// Conversion operator to Tcl_Obj*.
  operator Tcl_ObjPtr() { return itsObj; }

  bool isShared() const;
  bool isUnique() const { return !isShared(); }

  void ensureUnique();

private:
  static void incrRef(Tcl_Obj* obj);
  static void decrRef(Tcl_Obj* obj);

  void assign(Tcl_Obj* x)
    {
      if (itsObj != x)
        {
          decrRef(itsObj);
          itsObj = x;
          incrRef(itsObj);
        }
    }

  Tcl_Obj* itsObj;
};

} // end namespace Tcl

static const char vcid_tclobjptr_h[] = "$Header$";
#endif // !TCLOBJPTR_H_DEFINED
