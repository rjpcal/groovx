///////////////////////////////////////////////////////////////////////
//
// rhtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:39:46 1999
// written: Tue Jun 29 18:34:19 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHTCL_CC_DEFINED
#define RHTCL_CC_DEFINED

#include <tcl.h>

#include "rhlist.h"
#include "responsehandler.h"
#include "kbdresponsehdlr.h"
#include "nullresponsehdlr.h"
#include "tclcmd.h"
#include "tclitempkg.h"
#include "listpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace RhListTcl {
  class RhListPkg;
}

namespace KbdRhTcl {
  class KbdResponseHdlrCmd;
  class KbdRhPkg;
}

namespace NullRhTcl {
  class NullResponseHdlrCmd;
  class NullRhPkg;
}

//--------------------------------------------------------------------
//
// KbdResponseHdlrCmd --
//
//--------------------------------------------------------------------

class KbdRhTcl::KbdResponseHdlrCmd : public TclCmd {
public:
  KbdResponseHdlrCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 KbdResponseHdlr* rh = new KbdResponseHdlr();
	 int rhid = RhList::theRhList().insert(rh);
	 returnInt(rhid);
  }
};

///////////////////////////////////////////////////////////////////////
//
// KbdRhPkg class definition
//
///////////////////////////////////////////////////////////////////////

class KbdRhTcl::KbdRhPkg : public CTclIoItemPkg<KbdResponseHdlr> {
public:
  KbdRhPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<KbdResponseHdlr>(interp, "KbdRh", "1.4")
  {
	 addCommand( new KbdResponseHdlrCmd(interp, "KbdRh::kbdResponseHdlr") );
	 declareAttrib("useFeedback",
						new CAttrib<KbdResponseHdlr, bool> (
										&KbdResponseHdlr::getUseFeedback,
										&KbdResponseHdlr::setUseFeedback));
	 declareAttrib("keyRespPairs",
						new CAttrib<KbdResponseHdlr, const string&> (
									   &KbdResponseHdlr::getKeyRespPairs,
										&KbdResponseHdlr::setKeyRespPairs));
  }

  virtual KbdResponseHdlr* getCItemFromId(int id) {
	 if ( !RhList::theRhList().isValidId(id) ) {
		throw TclError("invalid response handler id");
	 }
	 KbdResponseHdlr* p = 
		dynamic_cast<KbdResponseHdlr*>(RhList::theRhList().getPtr(id));
	 if (p == 0) {
		throw TclError("response handler not of correct type");
	 }
	 return p;
  }

  virtual IO& getIoFromId(int id) {
	 return dynamic_cast<IO&>( *(getCItemFromId(id)) );
  }
};

//--------------------------------------------------------------------
//
// NullResponseHdlrCmd --
//
//--------------------------------------------------------------------

class NullRhTcl::NullResponseHdlrCmd : public TclCmd {
public:
  NullResponseHdlrCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 NullResponseHdlr* rh = new NullResponseHdlr();
	 int rhid = RhList::theRhList().insert(rh);
	 returnInt(rhid);
  }
};

///////////////////////////////////////////////////////////////////////
//
// NullRhPkg class definition
//
///////////////////////////////////////////////////////////////////////

class NullRhTcl::NullRhPkg : public CTclIoItemPkg<NullResponseHdlr> {
public:
  NullRhPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<NullResponseHdlr>(interp, "NullRh", "1.4")
  {
	 addCommand( new NullResponseHdlrCmd(interp, "NullRh::nullResponseHdlr") );
  }

  virtual NullResponseHdlr* getCItemFromId(int id) {
	 if ( !RhList::theRhList().isValidId(id) ) {
		throw TclError("invalid response handler id");
	 }
	 NullResponseHdlr* p = 
		dynamic_cast<NullResponseHdlr*>(RhList::theRhList().getPtr(id));
	 if (p == 0) {
		throw TclError("response handler not of correct type");
	 }
	 return p;
  }

  virtual IO& getIoFromId(int id) {
	 return dynamic_cast<IO&>( *(getCItemFromId(id)) );
  }
};

///////////////////////////////////////////////////////////////////////
//
// RhListPkg class definition
//
///////////////////////////////////////////////////////////////////////

class RhListTcl::RhListPkg : public ListPkg<RhList> {
public:
  RhListPkg(Tcl_Interp* interp) :
	 ListPkg<RhList>(interp, "RhList", "3.0")
  {
	 RhList::theRhList().setInterp(interp);
	 RhList::theRhList().insertAt(0, new KbdResponseHdlr());
  }

  virtual IO& getIoFromId(int) { return RhList::theRhList(); }

  virtual RhList* getCItemFromId(int) {
	 return &RhList::theRhList();
  }
};

//--------------------------------------------------------------------
//
// Rh_Init --
//
//--------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Rh_Init;

int Rh_Init(Tcl_Interp* interp) {
DOTRACE("Rh_Init");

  new RhListTcl::RhListPkg(interp);
  new KbdRhTcl::KbdRhPkg(interp);
  new NullRhTcl::NullRhPkg(interp);

  return TCL_OK;
}

static const char vcid_rhtcl_cc[] = "$Header$";
#endif // !RHTCL_CC_DEFINED
