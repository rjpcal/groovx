///////////////////////////////////////////////////////////////////////
//
// rhtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun  9 20:39:46 1999
// written: Wed Jul 18 10:41:19 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHTCL_CC_DEFINED
#define RHTCL_CC_DEFINED

#include "eventresponsehdlr.h"
#include "responsehandler.h"
#include "kbdresponsehdlr.h"
#include "nullresponsehdlr.h"

#include "tcl/tclitempkg.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"
#include "util/pointers.h"
#include "util/serialport.h"
#include "util/strings.h"

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

namespace SerialRhTcl
{
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

namespace SerialRhTcl
{
  shared_ptr<SerialEventSource> theEventSource;

  void startSerial(Tcl::Context& ctx)
  {
    const char* device = ctx.objc() >= 2 ?
      ctx.getCstringFromArg(1) : "/dev/tty0p0";

    theEventSource.reset(new SerialEventSource(ctx.interp(), device));
  }
}

class SerialRhTcl::SerialRhPkg : public Tcl::TclItemPkg {
public:
  SerialRhPkg(Tcl_Interp* interp) :
    Tcl::TclItemPkg(interp, "SerialRh", "$Revision$")
    {
      defRaw( &SerialRhTcl::startSerial, "SerialRh::SerialRh",
              "device=/dev/tty0p0", 1 );
      defRaw( &SerialRhTcl::startSerial, "SerialRh::SerialRh",
              0, 0 );
    }
};

///////////////////////////////////////////////////////////////////////
//
// EventRhPkg class definition
//
///////////////////////////////////////////////////////////////////////

namespace EventRhTcl
{
  class EventRhPkg;
}

class EventRhTcl::EventRhPkg : public Tcl::TclItemPkg {
public:
  EventRhPkg(Tcl_Interp* interp) :
    Tcl::TclItemPkg(interp, "EventRh", "$Revision$")
  {
    Tcl::defTracing(this, EventResponseHdlr::tracer);

    Tcl::defGenericObjCmds<EventResponseHdlr>(this);

    defAction("abortInvalidResponses",
              &EventResponseHdlr::abortInvalidResponses);
    defAction("ignoreInvalidResponses",
              &EventResponseHdlr::ignoreInvalidResponses);
    defAttrib("useFeedback",
              &EventResponseHdlr::getUseFeedback,
              &EventResponseHdlr::setUseFeedback);
    defAttrib("inputResponseMap",
              &EventResponseHdlr::getInputResponseMap,
              &EventResponseHdlr::setInputResponseMap);
    defAttrib("feedbackMap",
              &EventResponseHdlr::getFeedbackMap,
              &EventResponseHdlr::setFeedbackMap);
    defAttrib("eventSequence",
              &EventResponseHdlr::getEventSequence,
              &EventResponseHdlr::setEventSequence);
    defAttrib("bindingSubstitution",
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

class KbdRhTcl::KbdRhPkg : public Tcl::TclItemPkg {
public:
  KbdRhPkg(Tcl_Interp* interp) :
    Tcl::TclItemPkg(interp, "KbdRh", "$Revision$")
  {
    Tcl::defGenericObjCmds<KbdResponseHdlr>(this);

    Tcl_Eval(interp,
             "namespace eval KbdRh {\n"
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


//--------------------------------------------------------------------
//
// Rh_Init --
//
//--------------------------------------------------------------------

extern "C"
int Rh_Init(Tcl_Interp* interp) {
DOTRACE("Rh_Init");

  Tcl::TclPkg* pkg1 = new Tcl::TclItemPkg(interp, "Rh", "$Revision$");
  Tcl::defGenericObjCmds<ResponseHandler>(pkg1);

  Tcl::TclPkg* pkg2 = new EventRhTcl::EventRhPkg(interp);
  Tcl::TclPkg* pkg3 = new KbdRhTcl::KbdRhPkg(interp);
  Tcl::TclPkg* pkg4 = new Tcl::TclItemPkg(interp, "NullRh",
                                          "$Revision$");

  Tcl::TclPkg* pkg5 = new SerialRhTcl::SerialRhPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&EventResponseHdlr::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&KbdResponseHdlr::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&NullResponseHdlr::make);

  return pkg1->combineStatus(
         pkg2->combineStatus(
         pkg3->combineStatus(
         pkg4->combineStatus(
         pkg5->initStatus()))));
}

static const char vcid_rhtcl_cc[] = "$Header$";
#endif // !RHTCL_CC_DEFINED
