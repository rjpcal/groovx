///////////////////////////////////////////////////////////////////////
//
// gtexttcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 12:30:38 1999
// written: Mon Sep 27 14:34:37 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXTTCL_CC_DEFINED
#define GTEXTTCL_CC_DEFINED

#include <tcl.h>

#include "iomgr.h"
#include "objlist.h"
#include "gtext.h"
#include "listitempkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace GtextTcl {
  class GtextCmd;
  class GtextPkg;
}

//---------------------------------------------------------------------
//
// GtextTcl::GtextCmd --
//
//---------------------------------------------------------------------

class GtextTcl::GtextCmd : public TclCmd {
public:
  GtextCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "?text?", 1, 2, false) {}
protected:
  virtual void invoke() {
	 const char* text = (objc() < 2) ? 0 : getCstringFromArg(1);

	 Gtext* p = new Gtext(text);

	 int objid = ObjList::theObjList().insert(p);
	 returnInt(objid);
  }
};

//---------------------------------------------------------------------
//
// GtextTcl::GtextPkg --
//
//---------------------------------------------------------------------

class GtextTcl::GtextPkg : public ListItemPkg<Gtext, ObjList> {
public:
  GtextPkg(Tcl_Interp* interp) :
	 ListItemPkg<Gtext, ObjList>(interp, ObjList::theObjList(), "Gtext", "1.1")
  {
	 addCommand( new GtextCmd(interp, "Gtext::Gtext") );
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

extern "C" Tcl_PackageInitProc Gtext_Init;

int Gtext_Init(Tcl_Interp* interp) {
DOTRACE("Gtext_Init");

  new GtextTcl::GtextPkg(interp); 

  FactoryRegistrar<IO, Gtext> registrar(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_gtexttcl_cc[] = "$Header$";
#endif // !GTEXTTCL_CC_DEFINED
