/** @file gfx/tclpkg-toglet.h tcl interface package for Toglet */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Sat Jun 25 16:59:05 2005
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

#ifndef GROOVX_GFX_TCLPKG_TOGLET_H_UTC20050628170310_DEFINED
#define GROOVX_GFX_TCLPKG_TOGLET_H_UTC20050628170310_DEFINED

struct Tcl_Interp;

extern "C" int Toglet_Init(Tcl_Interp* interp);

#endif // !GROOVX_GFX_TCLPKG_TOGLET_H_UTC20050628170310_DEFINED
