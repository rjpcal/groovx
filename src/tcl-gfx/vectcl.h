/** @file gfx/vectcl.h tcl conversions to/from geom::vec2 */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Nov 20 14:06:16 2004
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

#ifndef GROOVX_GFX_VECTCL_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_VECTCL_H_UTC20050626084023_DEFINED

#include "geom/vec2.h"
#include "geom/vec3.h"

#include "tcl/conversions.h"
#include "tcl/list.h"

namespace tcl
{
  template <class T>
  struct help_convert<geom::vec2<T> >
  {
    /// Conversion routine for Tcl object to geom::vec2.
    static geom::vec2<T> from_tcl(Tcl_Obj* obj)
    {
      tcl::list listObj(obj);

      return geom::vec2<T>(listObj.template get<T>(0),
                           listObj.template get<T>(1));
    }

    /// Conversion routine for geom::vec2 to tcl::obj.
    static tcl::obj to_tcl( const geom::vec2<T>& v )
    {
      tcl::list listObj;
      listObj.append(v.x());
      listObj.append(v.y());
      return listObj.as_obj();
    }
  };

  template <class T>
  struct help_convert<geom::vec3<T> >
  {
    /// Conversion routine for Tcl object to geom::vec3.
    static geom::vec3<T> from_tcl(Tcl_Obj* obj)
    {
      tcl::list listObj(obj);

      return geom::vec3<T>(listObj.template get<T>(0),
                           listObj.template get<T>(1),
                           listObj.template get<T>(2));
    }

    /// Conversion routine for geom::vec3 to tcl::obj.
    static tcl::obj to_tcl( const geom::vec3<T>& v )
    {
      tcl::list listObj;
      listObj.append(v.x());
      listObj.append(v.y());
      listObj.append(v.z());
      return listObj.as_obj();
    }
  };
}

static const char __attribute__((used)) vcid_groovx_gfx_vectcl_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_VECTCL_H_UTC20050626084023_DEFINED
