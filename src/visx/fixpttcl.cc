///////////////////////////////////////////////////////////////////////
//
// fixpttcl.cc
// Rob Peters
// created: Jan-99
// written: Mon Jul 12 13:04:29 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPTTCL_CC_DEFINED
#define FIXPTTCL_CC_DEFINED

#include <tcl.h>
#include <cstring>

#include "iomgr.h"
#include "objlist.h"
#include "fixpt.h"
#include "listitempkg.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// FixptPkg class definition
//
///////////////////////////////////////////////////////////////////////

namespace FixptTcl {
  class FixptPkg;
}

class FixptTcl::FixptPkg : public ListItemPkg<FixPt, ObjList> {
public:
  FixptPkg(Tcl_Interp* interp) :
	 ListItemPkg<FixPt, ObjList>(interp, ObjList::theObjList(), "Fixpt", "2.6")
  {
	 declareCAttrib("length", &FixPt::getLength, &FixPt::setLength);
	 declareCAttrib("width", &FixPt::getWidth, &FixPt::setWidth);
  }
};

//---------------------------------------------------------------------
//
// Fixpt_Init --
//
//---------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Fixpt_Init;

int Fixpt_Init(Tcl_Interp* interp) {
DOTRACE("Fixpt_Init");

  new FixptTcl::FixptPkg(interp);

  FactoryRegistrar<IO, FixPt> registrar(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_fixpttcl_cc[] = "$Header$";
#endif // !FIXPTTCL_CC_DEFINED
