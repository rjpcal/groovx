/** @file io/fieldpkg.h tcl interfaces for manipulating FieldContainer
    fields */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sun Nov 12 17:45:52 2000
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

#ifndef GROOVX_TCL_IO_FIELDPKG_H_UTC20050712162004_DEFINED
#define GROOVX_TCL_IO_FIELDPKG_H_UTC20050712162004_DEFINED

#include "tcl/objpkg.h"
#include "tcl/pkg.h"

namespace rutz
{
  class file_pos;
}

class Field;
class FieldMap;

namespace tcl
{
  class pkg;

  void defField(pkg* pkg, const Field& field,
                const rutz::file_pos& src_pos);
  void defAllFields(pkg* pkg, const FieldMap& fmap,
                    const rutz::file_pos& src_pos);

  template <class C>
  void defFieldContainer(pkg* pkg, const rutz::file_pos& src_pos)
  {
    tcl::def_basic_type_cmds<C>(pkg, src_pos);

    tcl::defAllFields(pkg, C::classFields(), src_pos);
  }
}

static const char __attribute__((used)) vcid_groovx_tcl_io_fieldpkg_h_utc20050712162004[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_IO_FIELDPKG_H_UTC20050712162004_DEFINED
