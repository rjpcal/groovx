///////////////////////////////////////////////////////////////////////
//
// gtexttcl.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 12:30:38 1999
// written: Mon Dec 11 14:29:48 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXTTCL_CC_DEFINED
#define GTEXTTCL_CC_DEFINED

#include "gtext.h"

#include "io/iofactory.h"

#include "tcl/ioitempkg.h"

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

class GtextTcl::GtextPkg : public Tcl::IoItemPkg<Gtext> {
public:
  GtextPkg(Tcl_Interp* interp) :
	 Tcl::IoItemPkg<Gtext>(interp, "Gtext", "$Revision$")
  {
	 declareCAttrib("text", &Gtext::getText, &Gtext::setText);
	 declareCAttrib("strokeWidth",
						 &Gtext::getStrokeWidth, &Gtext::setStrokeWidth);
  }
};

//---------------------------------------------------------------------
//
// Gtext_Init --
//
//---------------------------------------------------------------------

extern "C"
int Gtext_Init(Tcl_Interp* interp) {
DOTRACE("Gtext_Init");

  Tcl::TclPkg* pkg = new GtextTcl::GtextPkg(interp); 

  IO::IoFactory::theOne().registerCreatorFunc(&Gtext::make);

  return pkg->initStatus();
}

static const char vcid_gtexttcl_cc[] = "$Header$";
#endif // !GTEXTTCL_CC_DEFINED
