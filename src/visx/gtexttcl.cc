///////////////////////////////////////////////////////////////////////
//
// gtexttcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 12:30:38 1999
// written: Thu Jul  1 13:07:54 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXTTCL_CC_DEFINED
#define GTEXTTCL_CC_DEFINED

#include <tcl.h>

#include "iomgr.h"
#include "objlist.h"
#include "gtext.h"
#include "tclitempkg.h"

#define LOCAL_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace GtextTcl {
  class GtextCmd;
  class LoadFontCmd;
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

	 ObjId objid = ObjList::theObjList().addObj(p);
	 returnInt(objid.toInt());
  }
};

//---------------------------------------------------------------------
//
// GtextTcl::LoadFontCmd --
//
//---------------------------------------------------------------------

class GtextTcl::LoadFontCmd : public TclCmd {
public:
  LoadFontCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "?fontname?", 1, 2) {}
protected:
  virtual void invoke() {
	 const char* fontname = (objc() < 2) ? 0 : getCstringFromArg(1);

	 Gtext::loadFont(fontname);
	 returnVoid();
  }
};

//---------------------------------------------------------------------
//
// GtextTcl::GtextPkg --
//
//---------------------------------------------------------------------

class GtextTcl::GtextPkg : public CTclIoItemPkg<Gtext> {
public:
  GtextPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<Gtext>(interp, "Gtext", "1.1")
  {
	 addCommand( new GtextCmd(interp, "Gtext::Gtext") );
	 addCommand( new LoadFontCmd(interp, "Gtext::loadFont") );
	 declareAttrib("text", new CAttrib<Gtext, const char*>(&Gtext::getText,
																			 &Gtext::setText));
  }

  virtual Gtext* getCItemFromId(int id) {
	 ObjId objid(id);
	 if ( !objid ) {
		throw TclError("objid out of range");
	 }
	 Gtext* p = dynamic_cast<Gtext*>(objid.get());
	 if ( p == NULL ) {
		throw TclError("object not of correct type");
	 }
	 return p;
  }

  virtual IO& getIoFromId(int id) {
	 return dynamic_cast<IO&>( *(getCItemFromId(id)) );
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
