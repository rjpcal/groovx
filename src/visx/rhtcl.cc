///////////////////////////////////////////////////////////////////////
//
// rhtcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun  9 20:39:46 1999
// written: Wed Mar 19 12:46:29 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHTCL_CC_DEFINED
#define RHTCL_CC_DEFINED

#include "visx/eventresponsehdlr.h"
#include "visx/responsehandler.h"
#include "visx/kbdresponsehdlr.h"
#include "visx/nullresponsehdlr.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"
#include "util/pointers.h"
#include "util/serialport.h"
#include "util/strings.h"

#include <tk.h>
#include <X11/Xlib.h>

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Serial port response handling
//
///////////////////////////////////////////////////////////////////////

namespace
{

  class SerialEventSource
  {
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

    static void setupProc(ClientData /*clientData*/, int flags)
    {
      if ( !(flags & TCL_FILE_EVENTS) ) return;

      Tcl_Time block_time;
      block_time.sec = 0;
      block_time.usec = 1000;

      Tcl_SetMaxBlockTime(&block_time);
    }

    static void checkProc(ClientData clientData, int flags)
    {
      if ( !(flags & TCL_FILE_EVENTS) ) return;

      SerialEventSource* source = static_cast<SerialEventSource*>(clientData);

      if ( !source->itsPort.isClosed() )
        {
          int n;
          while ( (n = source->itsPort.get()) != EOF)
            {
              if ( n >= 'A' && n <= 'H' )
                {
                  dbgEvalNL(3, (n-'A'));

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


  shared_ptr<SerialEventSource> theEventSource;

  void startSerial(Tcl_Interp* interp, const char* device)
  {
    theEventSource.reset(new SerialEventSource(interp, device));
  }
}


//--------------------------------------------------------------------
//
// Rh_Init --
//
//--------------------------------------------------------------------

extern "C"
int Rh_Init(Tcl_Interp* interp)
{
DOTRACE("Rh_Init");

  //
  // Rh
  //

  Tcl::Pkg* pkg1 = new Tcl::Pkg(interp, "ResponseHandler", "$Revision$");
  pkg1->inherit("IO");
  Tcl::defGenericObjCmds<ResponseHandler>(pkg1);
  pkg1->namespaceAlias("Rh");

  //
  // EventRh
  //

  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "EventResponseHdlr",
                                "$Revision$");
  pkg2->inherit("ResponseHandler");
  Tcl::defTracing(pkg2, EventResponseHdlr::tracer);

  Tcl::defGenericObjCmds<EventResponseHdlr>(pkg2);

  pkg2->defAction("abortInvalidResponses",
                  &EventResponseHdlr::abortInvalidResponses);
  pkg2->defAction("ignoreInvalidResponses",
                  &EventResponseHdlr::ignoreInvalidResponses);
  pkg2->defAttrib("useFeedback",
                  &EventResponseHdlr::getUseFeedback,
                  &EventResponseHdlr::setUseFeedback);
  pkg2->defSetter("inputResponseMap",
                  &EventResponseHdlr::setInputResponseMap);
  pkg2->defAttrib("feedbackMap",
                  &EventResponseHdlr::getFeedbackMap,
                  &EventResponseHdlr::setFeedbackMap);
  pkg2->defAttrib("eventSequence",
                  &EventResponseHdlr::getEventSequence,
                  &EventResponseHdlr::setEventSequence);
  pkg2->defAttrib("bindingSubstitution",
                  &EventResponseHdlr::getBindingSubstitution,
                  &EventResponseHdlr::setBindingSubstitution);
  pkg2->def("responseProc", "", &EventResponseHdlr::getResponseProc);
  pkg2->def("responseProc", "args body", &EventResponseHdlr::setResponseProc);
  pkg2->defAttrib("maxResponses",
                  &EventResponseHdlr::getMaxResponses,
                  &EventResponseHdlr::setMaxResponses);

  pkg2->namespaceAlias("EventRh");

  //
  // KbdRh
  //

  Tcl::Pkg* pkg3 = new Tcl::Pkg(interp, "KbdResponseHdlr", "$Revision$");
  pkg3->inherit("EventResponseHdlr");
  Tcl::defGenericObjCmds<KbdResponseHdlr>(pkg3);

  pkg3->namespaceAlias("KbdRh");

  //
  // NullRh
  //

  Tcl::Pkg* pkg4 = new Tcl::Pkg(interp, "NullResponseHdlr",
                                "$Revision$");
  pkg4->inherit("ResponseHandler");
  Tcl::defGenericObjCmds<NullResponseHdlr>(pkg4);
  pkg4->namespaceAlias("NullRh");

  //
  // SerialRh
  //

  Tcl::Pkg* pkg5 = new Tcl::Pkg(interp, "SerialRh", "$Revision$");
  pkg5->def( "SerialRh::SerialRh", "device=/dev/tty0p0",
             Util::bindFirst(&startSerial, interp) );
  pkg5->def( "SerialRh::SerialRh", "",
             Util::bindLast(Util::bindFirst(&startSerial, interp),
                            "/dev/tty0p0") );

  Util::ObjFactory::theOne().registerCreatorFunc(&EventResponseHdlr::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&KbdResponseHdlr::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&NullResponseHdlr::make);

  return Tcl::Pkg::initStatus(pkg1, pkg2, pkg3, pkg4, pkg5);
}

static const char vcid_rhtcl_cc[] = "$Header$";
#endif // !RHTCL_CC_DEFINED
