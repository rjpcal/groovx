/** @file pkgs/mtx/tclpkg-mtx.h tcl interface package for class mtx */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Jun 25 16:59:19 2005
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

#ifndef GROOVX_PKGS_MTX_TCLPKG_MTX_H_UTC20050628170148_DEFINED
#define GROOVX_PKGS_MTX_TCLPKG_MTX_H_UTC20050628170148_DEFINED

struct Tcl_Interp;

extern "C" int Mtx_Init(Tcl_Interp* interp);

static const char vcid_groovx_pkgs_mtx_tclpkg_mtx_h_utc20050628170148[] = "$Id$ $HeadURL$";
#endif // !GROOVX_PKGS_MTX_TCLPKG_MTX_H_UTC20050628170148_DEFINED
