///////////////////////////////////////////////////////////////////////
//
// tclconvert.h
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jul 11 08:57:31 2001
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

#ifndef GROOVX_TCL_CONVERSIONS_H_UTC20050628162420_DEFINED
#define GROOVX_TCL_CONVERSIONS_H_UTC20050628162420_DEFINED

#include "rutz/traits.h"

typedef struct Tcl_Obj Tcl_Obj;

namespace rutz
{
  class fstring;
  class value;
  template <class T> class fwd_iter;
}

namespace nub
{
  template <class T> class ref;
  template <class T> class soft_ref;
}

namespace Tcl
{
  class Dict;
  class List;
  class Obj;

  /// Trait class for extracting an appropriate return-type from T.
  template <class T>
  struct Return
  {
    // This machinery is simple to set up the rule that we want to
    // convert all rutz::value subclasses via strings. All other types
    // are converted directly.
    typedef typename rutz::select_if<
      rutz::is_sub_super<T, rutz::value>::result,
      rutz::fstring, T>::result_t
    Type;
  };

  /// Specialization of Tcl::Return for const T.
  template <class T>
  struct Return<const T>
  {
    typedef typename rutz::select_if<
      rutz::is_sub_super<T, rutz::value>::result,
      rutz::fstring, T>::result_t
    Type;
  };

  /// Specialization of Tcl::Return for const T&.
  template <class T>
  struct Return<const T&>
  {
    typedef typename rutz::select_if<
      rutz::is_sub_super<T, rutz::value>::result,
      rutz::fstring, T>::result_t
    Type;
  };

  //
  // Functions for converting from Tcl objects to C++ types.
  //

  Tcl_Obj*      fromTclImpl(Tcl_Obj* obj, Tcl_Obj**);
  Tcl::Obj      fromTclImpl(Tcl_Obj* obj, Tcl::Obj*);
  int           fromTclImpl(Tcl_Obj* obj, int*);
  unsigned int  fromTclImpl(Tcl_Obj* obj, unsigned int*);
  long          fromTclImpl(Tcl_Obj* obj, long*);
  unsigned long fromTclImpl(Tcl_Obj* obj, unsigned long*);
  bool          fromTclImpl(Tcl_Obj* obj, bool*);
  double        fromTclImpl(Tcl_Obj* obj, double*);
  float         fromTclImpl(Tcl_Obj* obj, float*);
  const char*   fromTclImpl(Tcl_Obj* obj, const char**);
  rutz::fstring fromTclImpl(Tcl_Obj* obj, rutz::fstring*);
  Tcl::Dict     fromTclImpl(Tcl_Obj* obj, Tcl::Dict*);
  Tcl::List     fromTclImpl(Tcl_Obj* obj, Tcl::List*);

  template <class T>
  inline typename Return<T>::Type fromTcl( Tcl_Obj* obj )
  {
    return fromTclImpl(obj, static_cast<typename Return<T>::Type*>(0));
  }

  //
  // Functions for converting from C++ types to Tcl objects.
  //

  Tcl::Obj toTclImpl(Tcl_Obj* val);
  Tcl::Obj toTclImpl(long val);
  Tcl::Obj toTclImpl(unsigned long val);
  Tcl::Obj toTclImpl(int val);
  Tcl::Obj toTclImpl(unsigned int val);
  Tcl::Obj toTclImpl(unsigned char val);
  Tcl::Obj toTclImpl(bool val);
  Tcl::Obj toTclImpl(double val);
  Tcl::Obj toTclImpl(float val);
  Tcl::Obj toTclImpl(const char* val);
  Tcl::Obj toTclImpl(const rutz::fstring& val);
  Tcl::Obj toTclImpl(const rutz::value& v);
  Tcl::Obj toTclImpl(Tcl::Dict dictObj);
  Tcl::Obj toTclImpl(Tcl::List listObj);
  Tcl::Obj toTclImpl(Tcl::Obj val);

  template <class T>
  inline Tcl::Obj toTcl(const T& val)
  {
    return toTclImpl(val);
  }
}

static const char vcid_groovx_tcl_conversions_h_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_CONVERSIONS_H_UTC20050628162420_DEFINED
