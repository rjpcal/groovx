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

namespace tcl
{
  class dict;
  class list;
  class obj;

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

  /// Specialization of tcl::Return for const T.
  template <class T>
  struct Return<const T>
  {
    typedef typename rutz::select_if<
      rutz::is_sub_super<T, rutz::value>::result,
      rutz::fstring, T>::result_t
    Type;
  };

  /// Specialization of tcl::Return for const T&.
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

  Tcl_Obj*      aux_convert_to(Tcl_Obj* obj, Tcl_Obj**);
  tcl::obj      aux_convert_to(Tcl_Obj* obj, tcl::obj*);
  int           aux_convert_to(Tcl_Obj* obj, int*);
  unsigned int  aux_convert_to(Tcl_Obj* obj, unsigned int*);
  long          aux_convert_to(Tcl_Obj* obj, long*);
  unsigned long aux_convert_to(Tcl_Obj* obj, unsigned long*);
  bool          aux_convert_to(Tcl_Obj* obj, bool*);
  double        aux_convert_to(Tcl_Obj* obj, double*);
  float         aux_convert_to(Tcl_Obj* obj, float*);
  const char*   aux_convert_to(Tcl_Obj* obj, const char**);
  rutz::fstring aux_convert_to(Tcl_Obj* obj, rutz::fstring*);
  tcl::dict     aux_convert_to(Tcl_Obj* obj, tcl::dict*);
  tcl::list     aux_convert_to(Tcl_Obj* obj, tcl::list*);

  template <class T>
  inline typename Return<T>::Type convert_to( Tcl_Obj* obj )
  {
    return aux_convert_to(obj, static_cast<typename Return<T>::Type*>(0));
  }

  //
  // Functions for converting from C++ types to Tcl objects.
  //

  tcl::obj aux_convert_from(Tcl_Obj* val);
  tcl::obj aux_convert_from(long val);
  tcl::obj aux_convert_from(unsigned long val);
  tcl::obj aux_convert_from(int val);
  tcl::obj aux_convert_from(unsigned int val);
  tcl::obj aux_convert_from(unsigned char val);
  tcl::obj aux_convert_from(bool val);
  tcl::obj aux_convert_from(double val);
  tcl::obj aux_convert_from(float val);
  tcl::obj aux_convert_from(const char* val);
  tcl::obj aux_convert_from(const rutz::fstring& val);
  tcl::obj aux_convert_from(const rutz::value& v);
  tcl::obj aux_convert_from(tcl::dict dict_value);
  tcl::obj aux_convert_from(tcl::list list_value);
  tcl::obj aux_convert_from(tcl::obj val);

  template <class T>
  inline tcl::obj convert_from(const T& val)
  {
    return aux_convert_from(val);
  }
}

static const char vcid_groovx_tcl_conversions_h_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_CONVERSIONS_H_UTC20050628162420_DEFINED
