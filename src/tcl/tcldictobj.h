///////////////////////////////////////////////////////////////////////
//
// tcldictobj.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon May 24 16:14:37 2004
// commit: $Id$
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

#ifndef TCLDICTOBJ_H_DEFINED
#define TCLDICTOBJ_H_DEFINED

#include "tcl/tclobjptr.h"

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

  Dict(const Tcl::ObjPtr& dictObj) :
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

  Tcl::ObjPtr asObj() const { return itsDictObj; }

private:
  void doPut(const char* key, Tcl::ObjPtr val);
  Tcl::ObjPtr doGet(const char* key) const;

  Tcl::ObjPtr itsDictObj;
};

static const char vcid_tcldictobj_h[] = "$Id$ $URL$";
#endif // !TCLDICTOBJ_H_DEFINED
