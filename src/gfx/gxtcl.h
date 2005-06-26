///////////////////////////////////////////////////////////////////////
//
// gxtcl.h
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Jun 25 16:58:56 2005
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

#ifndef GROOVX_GFX_GXTCL_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXTCL_H_UTC20050626084024_DEFINED

struct Tcl_Interp;

extern "C" int Gx_Init(Tcl_Interp* interp);
extern "C" int Gxnode_Init(Tcl_Interp* interp);
extern "C" int Gxseparator_Init(Tcl_Interp* interp);
extern "C" int Gxcolor_Init(Tcl_Interp* interp);
extern "C" int Gxdrawstyle_Init(Tcl_Interp* interp);
extern "C" int Gxline_Init(Tcl_Interp* interp);
extern "C" int Gxcylinder_Init(Tcl_Interp* interp);
extern "C" int Gxsphere_Init(Tcl_Interp* interp);
extern "C" int Gxlighting_Init(Tcl_Interp* interp);
extern "C" int Gxmaterial_Init(Tcl_Interp* interp);
extern "C" int Gxpointset_Init(Tcl_Interp* interp);
extern "C" int Gxscaler_Init(Tcl_Interp* interp);
extern "C" int Gxemptynode_Init(Tcl_Interp* interp);
extern "C" int Gxtransform_Init(Tcl_Interp* interp);
extern "C" int Gxshapekit_Init(Tcl_Interp* interp);
extern "C" int Gxpixmap_Init(Tcl_Interp* interp);
extern "C" int Gxtext_Init(Tcl_Interp* interp);
extern "C" int Gxfixedscalecamera_Init(Tcl_Interp* interp);
extern "C" int Gxpsyphycamera_Init(Tcl_Interp* interp);
extern "C" int Gxperspectivecamera_Init(Tcl_Interp* interp);
extern "C" int Gxdisk_Init(Tcl_Interp* interp);

static const char vcid_groovx_gfx_gxtcl_h_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXTCL_H_UTC20050626084024_DEFINED
