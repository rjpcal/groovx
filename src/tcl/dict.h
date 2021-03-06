/** @file tcl/dict.h c++ wrapper class for tcl "dict" objects (new in
    tcl 8.5) */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon May 24 16:14:37 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_TCL_DICT_H_UTC20050628162420_DEFINED
#define GROOVX_TCL_DICT_H_UTC20050628162420_DEFINED

#include "tcl/conversions.h"
#include "tcl/obj.h"

namespace tcl
{
  class dict;

  template <>
  struct help_convert<tcl::dict>
  {
    static tcl::dict from_tcl(Tcl_Obj* obj);
    static tcl::obj to_tcl(const tcl::dict& dict_value);
  };
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * tcl::dict class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

class tcl::dict
{
public:
  /// Default constructor makes an empty dict
  dict();

  dict(const tcl::obj& d) :
    m_obj(d)
  {}

  dict(const dict&) = default;
  dict& operator=(const dict&) = default;

  dict(dict&&) = default;
  dict& operator=(dict&&) = default;

  template <class T>
  void put(const char* key, const T& val)
  {
    do_put(key, tcl::convert_from(val));
  }

  template <class T>
  T get(const char* key) const
  {
    return tcl::convert_to<T>(do_get(key).get());
  }

  tcl::obj as_obj() const { return m_obj; }

private:
  void do_put(const char* key, const tcl::obj& val);
  tcl::obj do_get(const char* key) const;

  tcl::obj m_obj;
};

#endif // !GROOVX_TCL_DICT_H_UTC20050628162420_DEFINED
