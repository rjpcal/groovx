/** @file tcl/itertcl.h tcl converters for rutz::fwd_iter */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Aug 17 14:02:32 2001
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

#ifndef GROOVX_TCL_ITERTCL_H_UTC20050626084017_DEFINED
#define GROOVX_TCL_ITERTCL_H_UTC20050626084017_DEFINED

#include "rutz/iter.h"

#include "tcl/list.h"

namespace tcl
{
  template <class T>
  inline rutz::fwd_iter<T> aux_convert_to(Tcl_Obj* obj, rutz::fwd_iter<T>*)
  {
    tcl::list l(obj);
    return rutz::fwd_iter<T>(l.template begin<T>(), l.template end<T>());
  }

  template <class T>
  inline tcl::obj aux_convert_from( rutz::fwd_iter<T> iter )
  {
    tcl::list result;
    while ( !iter.at_end() )
      {
        result.append(*iter);
        iter.next();
      }
    return result.as_obj();
  }
}

static const char __attribute__((used)) vcid_groovx_tcl_itertcl_h_utc20050626084017[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_ITERTCL_H_UTC20050626084017_DEFINED
