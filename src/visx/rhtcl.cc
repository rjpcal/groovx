///////////////////////////////////////////////////////////////////////
//
// rhtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:39:46 1999
// written: Wed Mar 15 10:17:27 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHTCL_CC_DEFINED
#define RHTCL_CC_DEFINED

#include <tcl.h>

#include "eventresponsehdlr.h"
#include "iofactory.h"
#include "rhlist.h"
#include "responsehandler.h"
#include "kbdresponsehdlr.h"
#include "nullresponsehdlr.h"
#include "listitempkg.h"
#include "listpkg.h"
#include "tracertcl.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// EventRhPkg class definition
//
///////////////////////////////////////////////////////////////////////

namespace EventRhTcl {
  class EventRhPkg;
}

class EventRhTcl::EventRhPkg :
  public Tcl::ListItemPkg<EventResponseHdlr, RhList> {
public:
  EventRhPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<EventResponseHdlr, RhList>(interp, RhList::theRhList(),
																"EventRh", "$Revision$")
  {
	 addTracing(this, EventResponseHdlr::tracer);

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

class KbdRhTcl::KbdRhPkg : public Tcl::ListItemPkg<KbdResponseHdlr, RhList> {
public:
  KbdRhPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<KbdResponseHdlr, RhList>(interp, RhList::theRhList(),
															 "KbdRh", "$Revision$")
  {
	 Tcl_Eval(interp,
				 "namespace eval KbdRh {\n"
				 "  proc kbdResponseHdlr {} {return KbdRh}\n"
				 "  proc useFeedback args {\n"
				 "    return [eval EventRh::useFeedback $args]\n"
				 "  }\n"
				 "  proc keyRespPairs args {\n"
				 "    return [eval EventRh::inputResponseMap $args]\n"
				 "  }\n"
				 "  proc feedbackPairs args {\n"
				 "    return [eval EventRh::feedbackMap $args]\n"
				 "  }\n"
				 "}\n");
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

class NullRhTcl::NullRhPkg : public Tcl::ListItemPkg<NullResponseHdlr, RhList> {
public:
  NullRhPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<NullResponseHdlr, RhList>(
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

class RhListTcl::RhListPkg : public Tcl::IoPtrListPkg {
public:
  RhListPkg(Tcl_Interp* interp) :
	 Tcl::IoPtrListPkg(interp, RhList::theRhList(), "RhList", "$Revision$")
  {
	 RhList::theRhList().setInterp(interp);
	 RhList::theRhList().insertAt(0, RhList::Ptr(new KbdResponseHdlr()));
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

  new Tcl::AbstractListItemPkg<ResponseHandler, RhList>(
		  interp, RhList::theRhList(), "Rh", "$Revision$");
  new EventRhTcl::EventRhPkg(interp);
  new KbdRhTcl::KbdRhPkg(interp);
  new NullRhTcl::NullRhPkg(interp);


  FactoryRegistrar<IO, KbdResponseHdlr>   :: registerWith(IoFactory::theOne());
  FactoryRegistrar<IO, NullResponseHdlr>  :: registerWith(IoFactory::theOne());
  FactoryRegistrar<IO, EventResponseHdlr> :: registerWith(IoFactory::theOne());


  return TCL_OK;
}

static const char vcid_rhtcl_cc[] = "$Header$";
#endif // !RHTCL_CC_DEFINED
