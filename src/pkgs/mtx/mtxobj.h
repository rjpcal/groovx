///////////////////////////////////////////////////////////////////////
//
// mtxobj.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jul 10 13:07:53 2001
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

#ifndef MTXOBJ_H_DEFINED
#define MTXOBJ_H_DEFINED

#include "mtx.h"

#include "util/object.h"

/// Adapter class for fitting Mtx to the Util::Object interface.
class MtxObj : public Mtx, public Util::Object
{
public:
  template <class T1>
  MtxObj(T1 t1) : Mtx(t1) {}

  template <class T1, class T2>
  MtxObj(T1 t1, T2 t2) : Mtx(t1, t2) {}

  template <class T1, class T2, class T3>
  MtxObj(T1 t1, T2 t2, T3 t3) : Mtx(t1, t2, t3) {}

  template <class T1, class T2, class T3, class T4>
  MtxObj(T1 t1, T2 t2, T3 t3, T4 t4) : Mtx(t1, t2, t3, t4) {}

  virtual ~MtxObj() throw() {}

  static MtxObj* make() { return new MtxObj(0,0); }
};

static const char vcid_mtxobj_h[] = "$Header$";
#endif // !MTXOBJ_H_DEFINED
