///////////////////////////////////////////////////////////////////////
//
// rhtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:39:46 1999
// written: Fri Oct 27 16:01:39 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHTCL_CC_DEFINED
#define RHTCL_CC_DEFINED

#include "eventresponsehdlr.h"
#include "rhlist.h"
#include "responsehandler.h"
#include "kbdresponsehdlr.h"
#include "nullresponsehdlr.h"

#include "io/iofactory.h"

#include "tcl/listitempkg.h"
#include "tcl/listpkg.h"
#include "tcl/tracertcl.h"

#include "util/serialport.h"

#include <tk.h>
#include <X11/Xlib.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Serial port response handling
//
///////////////////////////////////////////////////////////////////////

namespace SerialRhTcl {
  class SerialEventSource;
  class SerialRhCmd;
  class SerialRhPkg;
}

class SerialRhTcl::SerialEventSource {
public:
  SerialEventSource(Tcl_Interp* interp, const char* serial_device) :
	 itsInterp(interp),
	 itsPort(serial_device)
	 {
		Tcl_CreateEventSource(setupProc, checkProc, static_cast<void*>(this));
	 }

  ~SerialEventSource()
	 {
		Tcl_DeleteEventSource(setupProc, checkProc, static_cast<void*>(this));
	 }

private:
  Tcl_Interp* itsInterp;
  Util::SerialPort itsPort;

  static void setupProc(ClientData /*clientData*/, int flags) {
	 if ( !(flags & TCL_FILE_EVENTS) ) return;

	 Tcl_Time block_time;
	 block_time.sec = 0;
	 block_time.usec = 1000;

	 Tcl_SetMaxBlockTime(&block_time);
  }

  static void checkProc(ClientData clientData, int flags) {
	 if ( !(flags & TCL_FILE_EVENTS) ) return;

	 SerialEventSource* source = static_cast<SerialEventSource*>(clientData);

	 if ( !source->itsPort.isClosed() )
		{
		  int n;
		  while ( (n = source->itsPort.get()) != EOF)
			 {
				if ( n >= 'A' && n <= 'H' )
				  {
					 DebugEvalNL((n-'A'));

					 Tk_FakeWin* tkwin = reinterpret_cast<Tk_FakeWin*>(
                                        Tk_MainWindow(source->itsInterp));
					 Display* display = Tk_Display(tkwin);

					 char keystring[2] = { char(n), '\0' };
					 KeySym keysym = XStringToKeysym(keystring);
					 KeyCode keycode = XKeysymToKeycode(display, keysym);

					 XEvent ev;
					 ev.xkey.type = KeyPress;
					 ev.xkey.send_event = True;
					 ev.xkey.display = display;
					 ev.xkey.window = Tk_WindowId(tkwin);
					 ev.xkey.keycode = keycode;
					 ev.xkey.state = 0;
					 Tk_QueueWindowEvent(&ev, TCL_QUEUE_TAIL);
				  }
			 }
		}
  }

private:
  SerialEventSource(const SerialEventSource&);
  SerialEventSource& operator=(const SerialEventSource&);  
};

class SerialRhTcl::SerialRhCmd : public Tcl::TclCmd {
public:
  SerialRhCmd(Tcl::TclPkg* pkg) :
	 Tcl::TclCmd(pkg->interp(), pkg->makePkgCmdName("SerialRh"),
			  "?serial_device = /dev/tty0p0", 1, 2),
	 itsEventSource(0)
	 {}
  virtual ~SerialRhCmd()
	 { delete itsEventSource; }

protected:
  virtual void invoke() 
	 {
		const char* device = objc() >= 2 ? getCstringFromArg(1) : "/dev/tty0p0";
		  
		itsEventSource = new SerialEventSource(interp(), device);
	 }

private:
  SerialRhCmd(const SerialRhCmd&);
  SerialRhCmd& operator=(const SerialRhCmd&);

  SerialEventSource* itsEventSource;
};

class SerialRhTcl::SerialRhPkg : public Tcl::TclPkg {
public:
  SerialRhPkg(Tcl_Interp* interp) :
	 Tcl::TclPkg(interp, "SerialRh", "$Revision$")
	 {
		addCommand( new SerialRhCmd(this) );
	 }
};

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
	 Tcl::addTracing(this, EventResponseHdlr::tracer);

	 declareCAction("abortInvalidResponses",
						 &EventResponseHdlr::abortInvalidResponses);
	 declareCAction("ignoreInvalidResponses",
						 &EventResponseHdlr::ignoreInvalidResponses);
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


//--------------------------------------------------------------------
//
// Rh_Init --
//
//--------------------------------------------------------------------

extern "C"
int Rh_Init(Tcl_Interp* interp) {
DOTRACE("Rh_Init");

  new Tcl::PtrListPkg<ResponseHandler>(interp, RhList::theRhList(),
													"RhList", "$Revision$");

  new Tcl::AbstractListItemPkg<ResponseHandler, RhList>(
		  interp, RhList::theRhList(), "Rh", "$Revision$");
  new EventRhTcl::EventRhPkg(interp);
  new KbdRhTcl::KbdRhPkg(interp);
  new NullRhTcl::NullRhPkg(interp);

  new SerialRhTcl::SerialRhPkg(interp);

  IO::IoFactory::theOne().registerCreatorFunc(&EventResponseHdlr::make);
  IO::IoFactory::theOne().registerCreatorFunc(&KbdResponseHdlr::make);
  IO::IoFactory::theOne().registerCreatorFunc(&NullResponseHdlr::make);

  return TCL_OK;
}

static const char vcid_rhtcl_cc[] = "$Header$";
#endif // !RHTCL_CC_DEFINED
