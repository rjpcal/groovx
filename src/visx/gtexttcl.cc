///////////////////////////////////////////////////////////////////////
//
// gtexttcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 12:30:38 1999
// written: Wed Jul 18 11:25:15 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXTTCL_CC_DEFINED
#define GTEXTTCL_CC_DEFINED

#include "gtext.h"

#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace GtextTcl {
  class GtextPkg;
}

//---------------------------------------------------------------------
//
// GtextTcl::GtextPkg --
//
//---------------------------------------------------------------------

class GtextTcl::GtextPkg : public Tcl::Pkg {
public:
  GtextPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Gtext", "$Revision$")
  {
    Tcl::defGenericObjCmds<Gtext>(this);

    defAttrib("text", &Gtext::getText, &Gtext::setText);
    defAttrib("strokeWidth",
              &Gtext::getStrokeWidth, &Gtext::setStrokeWidth);
  }
};

//---------------------------------------------------------------------
//
// Gtext_Init --
//
//---------------------------------------------------------------------

extern "C"
int Gtext_Init(Tcl_Interp* interp)
{
DOTRACE("Gtext_Init");

  Tcl::Pkg* pkg = new GtextTcl::GtextPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&Gtext::make);

  return pkg->initStatus();
}

static const char vcid_gtexttcl_cc[] = "$Header$";
#endif // !GTEXTTCL_CC_DEFINED
