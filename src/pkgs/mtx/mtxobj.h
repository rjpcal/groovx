///////////////////////////////////////////////////////////////////////
//
// mtxobj.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jul 10 13:07:53 2001
// written: Mon Jan 13 11:04:47 2003
// $Id$
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

  static MtxObj* make() { return new MtxObj(0,0); }
};

static const char vcid_mtxobj_h[] = "$Header$";
#endif // !MTXOBJ_H_DEFINED
