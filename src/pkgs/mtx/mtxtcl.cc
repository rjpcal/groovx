///////////////////////////////////////////////////////////////////////
//
// mtxtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul  9 17:49:07 2001
// written: Sat Aug 25 21:52:09 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MTXTCL_CC_DEFINED
#define MTXTCL_CC_DEFINED

#include "mtxobj.h"

#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include "util/trace.h"

extern "C"
int Mtx_Init(Tcl_Interp* interp)
{
DOTRACE("Mtx_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Mtx", "$Revision$");
  Tcl::defGenericObjCmds<MtxObj>(pkg);

  pkg->defAction<MtxObj>("print", &Mtx::print);

  pkg->defGetter<MtxObj, int>("mrows", &Mtx::mrows);
  pkg->defGetter<MtxObj, int>("ncols", &Mtx::ncols);
  pkg->defGetter<MtxObj, int>("nelems", &Mtx::nelems);

  Util::ObjFactory::theOne().registerCreatorFunc(&MtxObj::make);

  return pkg->initStatus();
}

static const char vcid_mtxtcl_cc[] = "$Header$";
#endif // !MTXTCL_CC_DEFINED
