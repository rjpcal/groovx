///////////////////////////////////////////////////////////////////////
//
// tclobjptr.h
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue May 11 13:44:19 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLOBJPTR_H_DEFINED
#define TCLOBJPTR_H_DEFINED

#include "tcl/tclconvert.h"

typedef struct Tcl_Obj Tcl_Obj;

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

  template <class T>
  T as() const
  {
    return Tcl::toNative<T>(itsObj);
  }

  void append(const Tcl::ObjPtr& other);

  template <class T>
  void append(const T& other)
  {
    append(Tcl::toTcl(other));
  }

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
