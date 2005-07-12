/** @file pkgs/mtx/mtxobj.h wrap mtx objects in a nub::object
    interface */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Jul 10 13:07:53 2001
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

#ifndef GROOVX_PKGS_MTX_MTXOBJ_H_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_MTXOBJ_H_UTC20050626084022_DEFINED

#include "mtx/mtx.h"

#include "nub/object.h"

#include "rutz/fstring.h"

/// Adapter class for fitting mtx to the nub::object interface.
class MtxObj : public mtx, public nub::object
{
public:
  template <class T1>
  MtxObj(T1 t1) : mtx(t1) {}

  template <class T1, class T2>
  MtxObj(T1 t1, T2 t2) : mtx(t1, t2) {}

  template <class T1, class T2, class T3>
  MtxObj(T1 t1, T2 t2, T3 t3) : mtx(t1, t2, t3) {}

  template <class T1, class T2, class T3, class T4>
  MtxObj(T1 t1, T2 t2, T3 t3, T4 t4) : mtx(t1, t2, t3, t4) {}

  virtual ~MtxObj() throw() {}

  static MtxObj* make() { return new MtxObj(mtx::empty_mtx()); }

  virtual rutz::fstring obj_typename() const { return "mtx"; }
};

static const char vcid_groovx_pkgs_mtx_mtxobj_h_utc20050626084022[] = "$Id$ $HeadURL$";
#endif // !GROOVX_PKGS_MTX_MTXOBJ_H_UTC20050626084022_DEFINED
