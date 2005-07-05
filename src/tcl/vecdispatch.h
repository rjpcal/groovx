///////////////////////////////////////////////////////////////////////
//
// tclveccmd.h
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Jul 12 12:14:43 2001
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

#ifndef GROOVX_TCL_VECDISPATCH_H_UTC20050628162421_DEFINED
#define GROOVX_TCL_VECDISPATCH_H_UTC20050628162421_DEFINED

namespace tcl
{
  class command;

  void use_vec_dispatch(tcl::command& cmd, unsigned int key_argn);
}

static const char vcid_groovx_tcl_vecdispatch_h_utc20050628162421[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_VECDISPATCH_H_UTC20050628162421_DEFINED