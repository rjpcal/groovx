/** @file tcl/dict.h c++ wrapper class for tcl "dict" objects (new in
    tcl 8.5) */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon May 24 16:14:37 2004
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

#ifndef GROOVX_TCL_DICT_H_UTC20050628162420_DEFINED
#define GROOVX_TCL_DICT_H_UTC20050628162420_DEFINED

#include "tcl/conversions.h"
#include "tcl/obj.h"

namespace tcl
{
  class dict;

  tcl::dict aux_convert_to(Tcl_Obj* obj, tcl::dict*);
  tcl::obj aux_convert_from(tcl::dict dict_value);
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

  dict(const dict& other) :
    m_obj(other.m_obj)
  {}

  dict& operator=(const dict& other)
  {
    m_obj = other.m_obj;
    return *this;
  };

  template <class T>
  void put(const char* key, const T& val)
  {
    do_put(key, tcl::convert_from(val));
  }

  template <class T>
  T get(const char* key, T* /*dummy*/=0) const
  {
    return tcl::convert_to<T>(do_get(key).get());
  }

  tcl::obj as_obj() const { return m_obj; }

private:
  void do_put(const char* key, tcl::obj val);
  tcl::obj do_get(const char* key) const;

  tcl::obj m_obj;
};

static const char __attribute__((used)) vcid_groovx_tcl_dict_h_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_DICT_H_UTC20050628162420_DEFINED
