///////////////////////////////////////////////////////////////////////
//
// gtexttcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 12:30:38 1999
// written: Wed Mar 29 23:57:00 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXTTCL_CC_DEFINED
#define GTEXTTCL_CC_DEFINED

#include "objlist.h"
#include "gtext.h"

#include "io/iofactory.h"

#include "tcl/listitempkg.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace GtextTcl {
  class GtextCmd;
  class GtextPkg;
}

//---------------------------------------------------------------------
//
// GtextTcl::GtextCmd --
//
//---------------------------------------------------------------------

class GtextTcl::GtextCmd : public Tcl::TclCmd {
public:
  GtextCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name, "?text?", 1, 2, false) {}
protected:
  virtual void invoke() {
	 const char* text = (objc() < 2) ? 0 : getCstringFromArg(1);

	 Gtext* p = new Gtext(text);

	 int objid = ObjList::theObjList().insert(ObjList::Ptr(p));
	 returnInt(objid);
  }
};

//---------------------------------------------------------------------
//
// GtextTcl::GtextPkg --
//
//---------------------------------------------------------------------

class GtextTcl::GtextPkg : public Tcl::ListItemPkg<Gtext, ObjList> {
public:
  GtextPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<Gtext, ObjList>(interp, ObjList::theObjList(),
												 "Gtext", "1.1")
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

extern "C"
int Gtext_Init(Tcl_Interp* interp) {
DOTRACE("Gtext_Init");

  Tcl::TclPkg* pkg = new GtextTcl::GtextPkg(interp); 

  FactoryRegistrar<IO, Gtext>::registerWith(IoFactory::theOne());

  return pkg->initStatus();
}

static const char vcid_gtexttcl_cc[] = "$Header$";
#endif // !GTEXTTCL_CC_DEFINED
