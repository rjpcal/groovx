///////////////////////////////////////////////////////////////////////
//
// rhtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:39:46 1999
// written: Tue Nov  9 16:56:55 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHTCL_CC_DEFINED
#define RHTCL_CC_DEFINED

#include <tcl.h>

#include "eventresponsehdlr.h"
#include "iomgr.h"
#include "rhlist.h"
#include "responsehandler.h"
#include "kbdresponsehdlr.h"
#include "nullresponsehdlr.h"
#include "tclcmd.h"
#include "listitempkg.h"
#include "listpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// EventRhPkg class definition
//
///////////////////////////////////////////////////////////////////////

namespace EventRhTcl {
  class EventRhPkg;
}

class EventRhTcl::EventRhPkg : public ListItemPkg<EventResponseHdlr, RhList> {
public:
  EventRhPkg(Tcl_Interp* interp) :
	 ListItemPkg<EventResponseHdlr, RhList>(interp, RhList::theRhList(),
														 "EventRh", "$Revision$")
  {
	 declareCAttrib("useFeedback",
						 &EventResponseHdlr::getUseFeedback,
						 &EventResponseHdlr::setUseFeedback);
	 declareCAttrib("inputResponseMap",
						 &EventResponseHdlr::getInputResponseMap,
						 &EventResponseHdlr::setInputResponseMap);
	 declareCAttrib("feedbackMap",
						 &EventResponseHdlr::getFeedbackMap,
						 &EventResponseHdlr::setFeedbackMap);
	 declareCAttrib("eventSequence",
						 &EventResponseHdlr::getEventSequence,
						 &EventResponseHdlr::setEventSequence);
	 declareCAttrib("bindingSubstitution",
						 &EventResponseHdlr::getBindingSubstitution,
						 &EventResponseHdlr::setBindingSubstitution);
  }
};

///////////////////////////////////////////////////////////////////////
//
// KbdRhPkg class definition
//
///////////////////////////////////////////////////////////////////////

namespace KbdRhTcl {
  class KbdRhPkg;
}

class KbdRhTcl::KbdRhPkg : public ListItemPkg<KbdResponseHdlr, RhList> {
public:
  KbdRhPkg(Tcl_Interp* interp) :
	 ListItemPkg<KbdResponseHdlr, RhList>(interp, RhList::theRhList(),
													  "KbdRh", "$Revision$")
  {
	 declareCAttrib("useFeedback",
						 &KbdResponseHdlr::getUseFeedback,
						 &KbdResponseHdlr::setUseFeedback);
	 declareCAttrib("keyRespPairs",
						 &KbdResponseHdlr::getKeyRespPairs,
						 &KbdResponseHdlr::setKeyRespPairs);
	 declareCAttrib("feedbackPairs",
						 &KbdResponseHdlr::getFeedbackPairs,
						 &KbdResponseHdlr::setFeedbackPairs);

	 Tcl_Eval(interp,
			 "namespace eval KbdRh {proc kbdResponseHdlr {} {return KbdRh}}\n");
  }
};

///////////////////////////////////////////////////////////////////////
//
// NullRhPkg class definition
//
///////////////////////////////////////////////////////////////////////

namespace NullRhTcl {
  class NullRhPkg;
}

class NullRhTcl::NullRhPkg : public ListItemPkg<NullResponseHdlr, RhList> {
public:
  NullRhPkg(Tcl_Interp* interp) :
	 ListItemPkg<NullResponseHdlr, RhList>(
          interp, RhList::theRhList(), "NullRh", "$Revision$")
  {
	 Tcl_Eval(interp,
			"namespace eval NullRh {proc nullResponseHdlr {} {return NullRh}}");
  }
};

///////////////////////////////////////////////////////////////////////
//
// RhListPkg class definition
//
///////////////////////////////////////////////////////////////////////

namespace RhListTcl {
  class RhListPkg;
}

class RhListTcl::RhListPkg : public ListPkg<RhList> {
public:
  RhListPkg(Tcl_Interp* interp) :
	 ListPkg<RhList>(interp, RhList::theRhList(), "RhList", "$Revision$")
  {
	 RhList::theRhList().setInterp(interp);
	 RhList::theRhList().insertAt(0, new KbdResponseHdlr());
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

  new AbstractListItemPkg<ResponseHandler, RhList>(
		  interp, RhList::theRhList(), "Rh", "$Revision$");
  new EventRhTcl::EventRhPkg(interp);
  new KbdRhTcl::KbdRhPkg(interp);
  new NullRhTcl::NullRhPkg(interp);

  FactoryRegistrar<IO, KbdResponseHdlr> registrar1(IoFactory::theOne());
  FactoryRegistrar<IO, NullResponseHdlr> registrar2(IoFactory::theOne());
  FactoryRegistrar<IO, EventResponseHdlr> registrar3(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_rhtcl_cc[] = "$Header$";
#endif // !RHTCL_CC_DEFINED
