///////////////////////////////////////////////////////////////////////
//
// fieldpkg.h
//
// Copyright (c) 2000-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sun Nov 12 17:45:52 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_H_DEFINED
#define FIELDPKG_H_DEFINED

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

class Field;
class FieldMap;

namespace Tcl
{
  class Pkg;

  void defField(Pkg* pkg, const Field& field,
                const char* src_file_name,
                int src_line_no);
  void defAllFields(Pkg* pkg, const FieldMap& fmap,
                    const char* src_file_name,
                    int src_line_no);

  template <class C>
  void defFieldContainer(Pkg* pkg,
                         const char* src_file_name,
                         int src_line_no)
  {
    Tcl::defGenericObjCmds<C>(pkg, src_file_name, src_line_no);

    Tcl::defAllFields(pkg, C::classFields(),
                      src_file_name, src_line_no);
  }
}

static const char vcid_fieldpkg_h[] = "$Header$";
#endif // !FIELDPKG_H_DEFINED
