///////////////////////////////////////////////////////////////////////
//
// mtxtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul  9 17:49:07 2001
// written: Tue Jul 10 13:20:19 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTXTCL_CC_DEFINED
#define MTXTCL_CC_DEFINED

#include "mtxobj.h"

#include "tcl/genericobjpkg.h"

#include "util/objfactory.h"

#include "util/trace.h"

namespace MtxTcl
{
  class MtxPkg;
}

class MtxTcl::MtxPkg : public Tcl::GenericObjPkg<MtxObj> {
public:
  MtxPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<MtxObj>(interp, "Mtx", "$Revision$")
  {
    declareCAction("print", &Mtx::print);

    declareCGetter("mrows", &Mtx::mrows);
    declareCGetter("ncols", &Mtx::ncols);
    declareCGetter("nelems", &Mtx::nelems);
  }
};

extern "C"
int Mtx_Init(Tcl_Interp* interp) {
DOTRACE("Mtx_Init");

  Tcl::TclPkg* pkg = new MtxTcl::MtxPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&MtxObj::make);

  return pkg->initStatus();
}

static const char vcid_mtxtcl_cc[] = "$Header$";
#endif // !MTXTCL_CC_DEFINED
