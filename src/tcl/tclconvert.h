///////////////////////////////////////////////////////////////////////
//
// tclconvert.h
//
// Copyright (c) 2001-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jul 11 08:57:31 2001
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

#ifndef TCLCONVERT_H_DEFINED
#define TCLCONVERT_H_DEFINED

#include "util/traits.h"

class fstring;
class Value;
typedef struct Tcl_Obj Tcl_Obj;

namespace Util
{
  template <class T> class FwdIter;
  template <class T> class Ref;
  template <class T> class SoftRef;
}

namespace Tcl
{
  class Dict;
  class List;
  class ObjPtr;

  /// Trait class for extracting an appropriate return-type from T.
  template <class T>
  struct Return
  {
    // This machinery is simple to set up the rule that we want to convert
    // all Value subclasses via strings. All other types are converted
    // directly.
    typedef typename Util::SelectIf<Util::IsSubSuper<T, Value>::result,
                                    fstring, T>::Type Type;
  };

  /// Specialization of Tcl::Return for const T.
  template <class T>
  struct Return<const T>
  {
    typedef typename Util::SelectIf<Util::IsSubSuper<T, Value>::result,
                                    fstring, T>::Type Type;
  };

  /// Specialization of Tcl::Return for const T&.
  template <class T>
  struct Return<const T&>
  {
    typedef typename Util::SelectIf<Util::IsSubSuper<T, Value>::result,
                                    fstring, T>::Type Type;
  };

  //
  // Functions for converting from Tcl objects to C++ types.
  //

  Tcl_Obj*      fromTcl(Tcl_Obj* obj, Tcl_Obj**);
  Tcl::ObjPtr   fromTcl(Tcl_Obj* obj, Tcl::ObjPtr*);
  int           fromTcl(Tcl_Obj* obj, int*);
  unsigned int  fromTcl(Tcl_Obj* obj, unsigned int*);
  long          fromTcl(Tcl_Obj* obj, long*);
  unsigned long fromTcl(Tcl_Obj* obj, unsigned long*);
  bool          fromTcl(Tcl_Obj* obj, bool*);
  double        fromTcl(Tcl_Obj* obj, double*);
  float         fromTcl(Tcl_Obj* obj, float*);
  const char*   fromTcl(Tcl_Obj* obj, const char**);
  fstring       fromTcl(Tcl_Obj* obj, fstring*);
  Tcl::Dict     fromTcl(Tcl_Obj* obj, Tcl::Dict*);
  Tcl::List     fromTcl(Tcl_Obj* obj, Tcl::List*);

  template <class T>
  inline typename Return<T>::Type toNative( Tcl_Obj* obj )
  {
    return fromTcl(obj, (typename Return<T>::Type*)0);
  }

  //
  // Functions for converting from C++ types to Tcl objects.
  //

  Tcl::ObjPtr toTcl(Tcl_Obj* val);
  Tcl::ObjPtr toTcl(long val);
  Tcl::ObjPtr toTcl(unsigned long val);
  Tcl::ObjPtr toTcl(int val);
  Tcl::ObjPtr toTcl(unsigned int val);
  Tcl::ObjPtr toTcl(unsigned char val);
  Tcl::ObjPtr toTcl(bool val);
  Tcl::ObjPtr toTcl(double val);
  Tcl::ObjPtr toTcl(float val);
  Tcl::ObjPtr toTcl(const char* val);
  Tcl::ObjPtr toTcl(const fstring& val);
  Tcl::ObjPtr toTcl(const Value& val);
  Tcl::ObjPtr toTcl(Tcl::Dict dictObj);
  Tcl::ObjPtr toTcl(Tcl::List listObj);
  Tcl::ObjPtr toTcl(Tcl::ObjPtr val);

  template <class T>
  Util::Ref<T> fromTcl(Tcl_Obj* obj, Util::Ref<T>*);

  template <class T>
  Tcl::ObjPtr toTcl(Util::Ref<T> obj);

  template <class T>
  Util::SoftRef<T> fromTcl(Tcl_Obj* obj, Util::SoftRef<T>*);

  template <class T>
  Tcl::ObjPtr toTcl(Util::SoftRef<T> obj);

  template <class T>
  Util::FwdIter<T> fromTcl(Tcl_Obj* obj, Util::FwdIter<T>*);

  template <class T>
  Tcl::ObjPtr toTcl( Util::FwdIter<T> iter );
}

static const char vcid_tclconvert_h[] = "$Header$";
#endif // !TCLCONVERT_H_DEFINED
