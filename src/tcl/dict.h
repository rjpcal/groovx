///////////////////////////////////////////////////////////////////////
//
// tcldictobj.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon May 24 16:14:37 2004
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#include "tcl/obj.h"

namespace Tcl
{
  class Dict;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::Dict class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Dict
{
public:
  /// Default constructor makes an empty dict
  Dict();

  Dict(const Tcl::Obj& dictObj) :
    itsDictObj(dictObj)
  {}

  Dict(const Dict& other) :
    itsDictObj(other.itsDictObj)
  {}

  Dict& operator=(const Dict& other)
  {
    itsDictObj = other.itsDictObj;
    return *this;
  };

  template <class T>
  void put(const char* key, const T& val)
  {
    doPut(key, Tcl::toTcl(val));
  }

  template <class T>
  T get(const char* key, T* /*dummy*/=0) const
  {
    return Tcl::toNative<T>(doGet(key).obj());
  }

  Tcl::Obj asObj() const { return itsDictObj; }

private:
  void doPut(const char* key, Tcl::Obj val);
  Tcl::Obj doGet(const char* key) const;

  Tcl::Obj itsDictObj;
};

static const char vcid_groovx_tcl_dict_h_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_DICT_H_UTC20050628162420_DEFINED
