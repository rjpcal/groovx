///////////////////////////////////////////////////////////////////////
//
// tclobjptr.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue May 11 13:44:19 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_TCL_OBJ_H_UTC20050628162421_DEFINED
#define GROOVX_TCL_OBJ_H_UTC20050628162421_DEFINED

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

class Obj
{
public:
  /// Default constructor with a shared and empty Tcl_Obj*.
  Obj();

  /// Construct with a Tcl_Obj*.
  Obj(Tcl_Obj* obj) : itsObj(obj) { incrRef(itsObj); }

  /// Destructor.
  ~Obj() { decrRef(itsObj); }

  /// Copy constructor.
  Obj(const Obj& x) :
    itsObj(x.itsObj)
    {
      incrRef(itsObj);
    }

  /// Assignment operator from Obj.
  Obj& operator=(const Obj& x)
    {
      assign(x.itsObj); return *this;
    }

  /// Assignment operator from Tcl_Obj*.
  Obj& operator=(Tcl_Obj* x)
    {
      assign(x); return *this;
    }

  Tcl_Obj* obj() const { return itsObj; }

  template <class T>
  inline T as() const;

  void append(const Tcl::Obj& other);

  template <class T>
  inline void append(const T& other);

  bool is_shared() const;
  bool isUnique() const { return !is_shared(); }

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


///////////////////////////////////////////////////////////////////////
//
// Inline member definitions
//
///////////////////////////////////////////////////////////////////////

#include "tcl/conversions.h"

template <class T>
inline T Tcl::Obj::as() const
{
  return Tcl::fromTcl<T>(itsObj);
}

template <class T>
inline void Tcl::Obj::append(const T& other)
{
  append(Tcl::toTcl(other));
}

static const char vcid_groovx_tcl_obj_h_utc20050628162421[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_OBJ_H_UTC20050628162421_DEFINED
