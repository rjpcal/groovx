/** @file visx/tclpkg-trialevent.h tcl interface package for class
    TrialEvent and derivatives */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Sat Jun 25 16:59:22 2005
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

#ifndef GROOVX_VISX_TCLPKG_TRIALEVENT_H_UTC20050628171009_DEFINED
#define GROOVX_VISX_TCLPKG_TRIALEVENT_H_UTC20050628171009_DEFINED

struct Tcl_Interp;

extern "C" int Trialevent_Init(Tcl_Interp* interp);
extern "C" int Nulltrialevent_Init(Tcl_Interp* interp);
extern "C" int Filewriteevent_Init(Tcl_Interp* interp);
extern "C" int Genericevent_Init(Tcl_Interp* interp);
extern "C" int Multievent_Init(Tcl_Interp* interp);

#endif // !GROOVX_VISX_TCLPKG_TRIALEVENT_H_UTC20050628171009_DEFINED
