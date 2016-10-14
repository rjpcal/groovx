/** @file tcl/conversions.h tcl conversion functions for basic types */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
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
#include "tcl/obj.h"

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
  class obj;

  /// Trait class for extracting an appropriate return-type from T.
  template <class T>
  struct returnable
  {
    // This machinery is simple to set up the rule that we want to
    // convert all rutz::value subclasses via strings. All other types
    // are converted directly.
    typedef typename rutz::select_if<
      rutz::is_sub_super<T, rutz::value>::result,
      rutz::fstring, T>::result_t
    type;
  };

  /// Specialization of tcl::returnable for const T.
  template <class T>
  struct returnable<const T>
  {
    typedef typename rutz::select_if<
      rutz::is_sub_super<T, rutz::value>::result,
      rutz::fstring, T>::result_t
    type;
  };

  /// Specialization of tcl::returnable for const T&.
  template <class T>
  struct returnable<const T&>
  {
    typedef typename rutz::select_if<
      rutz::is_sub_super<T, rutz::value>::result,
      rutz::fstring, T>::result_t
    type;
  };

  //
  // Functions for converting from Tcl objects to C++ types.
  //

  template <class T>
  struct help_convert;

  /// Convert a tcl::obj to a native c++ object.
  /** Will select a matching help_convert<T> specialization. */
  template <class T>
  inline typename returnable<T>::type convert_to( const tcl::obj& obj )
  {
    return help_convert<typename returnable<T>::type>::from_tcl(obj.get());
  }

  /// Convert a tcl::obj to a native c++ object.
  /** Will select a matching help_convert<T> specialization. */
  template <class T>
  inline typename returnable<T>::type convert_to( Tcl_Obj* obj )
  {
    return help_convert<typename returnable<T>::type>::from_tcl(obj);
  }

  template <> struct help_convert<int          > { static int           from_tcl(Tcl_Obj* obj); static tcl::obj to_tcl(int val); };
  template <> struct help_convert<unsigned int > { static unsigned int  from_tcl(Tcl_Obj* obj); static tcl::obj to_tcl(unsigned int val); };
  template <> struct help_convert<long         > { static long          from_tcl(Tcl_Obj* obj); static tcl::obj to_tcl(long val); };
  template <> struct help_convert<unsigned long> { static unsigned long from_tcl(Tcl_Obj* obj); static tcl::obj to_tcl(unsigned long val); };
  template <> struct help_convert<long long    > { static long long     from_tcl(Tcl_Obj* obj); static tcl::obj to_tcl(long long val); };
  template <> struct help_convert<bool         > { static bool          from_tcl(Tcl_Obj* obj); static tcl::obj to_tcl(bool val); };
  template <> struct help_convert<double       > { static double        from_tcl(Tcl_Obj* obj); static tcl::obj to_tcl(double val); };
  template <> struct help_convert<float        > { static float         from_tcl(Tcl_Obj* obj); static tcl::obj to_tcl(float val); };
  template <> struct help_convert<const char*  > { static const char*   from_tcl(Tcl_Obj* obj); static tcl::obj to_tcl(const char* val); };
  template <> struct help_convert<rutz::fstring> { static rutz::fstring from_tcl(Tcl_Obj* obj); static tcl::obj to_tcl(const rutz::fstring& val); };
  template <> struct help_convert<unsigned char> {                                              static tcl::obj to_tcl(unsigned char val); };
  template <> struct help_convert<rutz::value>   {                                              static tcl::obj to_tcl(const rutz::value& val); };

  template <> struct help_convert<char*>
  {
    static tcl::obj to_tcl(char* val) { return help_convert<const char*>::to_tcl(val); }
  };

  template <unsigned int N> struct help_convert<char[N]>
  {
    static tcl::obj to_tcl(const char* val) { return help_convert<const char*>::to_tcl(val); }
  };

  template <> struct help_convert<Tcl_Obj*>
  {
    static Tcl_Obj* from_tcl(Tcl_Obj* obj) { return obj; }
    static tcl::obj to_tcl(Tcl_Obj* val) { return tcl::obj(val); }
  };

  template <> struct help_convert<tcl::obj>
  {
    static tcl::obj from_tcl(Tcl_Obj* obj) { return tcl::obj(obj); }
    static tcl::obj to_tcl(tcl::obj val) { return val; }
  };

  /// Convert a native c++ object to a tcl::obj.
  /** Will select a matching help_convert<T>::to_tcl() specialization. */
  template <class T>
  inline tcl::obj convert_from(const T& val)
  {
    return help_convert<T>::to_tcl(val);
  }
}

static const char __attribute__((used)) vcid_groovx_tcl_conversions_h_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_CONVERSIONS_H_UTC20050628162420_DEFINED
