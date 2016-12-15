/** @file visx/tclpkg-rh.cc tcl interface package for class
    ResponseHandler and derivatives */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Jun  9 20:39:46 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#include "visx/tclpkg-rh.h"

#include "visx/tclpkg-rh.h"

#include "nub/objfactory.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"
#include "tcl/tracertcl.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/serialport.h"

#include "visx/eventresponsehdlr.h"
#include "visx/responsehandler.h"
#include "visx/kbdresponsehdlr.h"
#include "visx/nullresponsehdlr.h"

#include <memory>
#include <tk.h>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

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
    rutz::serial_port itsPort;

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

      if ( !source->itsPort.is_closed() )
        {
          int n;
          while ( (n = source->itsPort.get()) != EOF)
            {
              if ( n >= 'A' && n <= 'H' )
                {
                  dbg_eval_nl(3, (n-'A'));

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

  std::shared_ptr<SerialEventSource> theEventSource;
}


//--------------------------------------------------------------------
//
// Package init procedures
//
//--------------------------------------------------------------------

extern "C"
int Responsehandler_Init(Tcl_Interp* interp)
{
GVX_TRACE("Responsehandler_Init");

  return tcl::pkg::init
    (interp, "ResponseHandler", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("io");
      tcl::def_basic_type_cmds<ResponseHandler>(pkg, SRC_POS);
      pkg->namesp_alias("Rh");
    });
}

extern "C"
int Eventresponsehdlr_Init(Tcl_Interp* interp)
{
GVX_TRACE("Eventresponsehdlr_Init");

  return tcl::pkg::init
    (interp, "EventResponseHdlr", "4.0",
     [](tcl::pkg* pkg) {
      tcl::def_creator<EventResponseHdlr>(pkg);
      pkg->inherit_pkg("ResponseHandler");
      tcl::def_tracing(pkg, EventResponseHdlr::tracer);

      tcl::def_basic_type_cmds<EventResponseHdlr>(pkg, SRC_POS);

      pkg->def_action("abortInvalidResponses",
                      &EventResponseHdlr::abortInvalidResponses,
                      SRC_POS);
      pkg->def_action("ignoreInvalidResponses",
                      &EventResponseHdlr::ignoreInvalidResponses,
                      SRC_POS);
      pkg->def_get_set("useFeedback",
                       &EventResponseHdlr::getUseFeedback,
                       &EventResponseHdlr::setUseFeedback,
                       SRC_POS);
      pkg->def_setter("inputResponseMap",
                      &EventResponseHdlr::setInputResponseMap,
                      SRC_POS);
      pkg->def_get_set("feedbackMap",
                       &EventResponseHdlr::getFeedbackMap,
                       &EventResponseHdlr::setFeedbackMap,
                       SRC_POS);
      pkg->def_get_set("eventSequence",
                       &EventResponseHdlr::getEventSequence,
                       &EventResponseHdlr::setEventSequence,
                       SRC_POS);
      pkg->def_get_set("bindingSubstitution",
                       &EventResponseHdlr::getBindingSubstitution,
                       &EventResponseHdlr::setBindingSubstitution,
                       SRC_POS);
      pkg->def("responseProc", "", &EventResponseHdlr::getResponseProc, SRC_POS);
      pkg->def("responseProc", "args body", &EventResponseHdlr::setResponseProc, SRC_POS);
      pkg->def_get_set("maxResponses",
                       &EventResponseHdlr::getMaxResponses,
                       &EventResponseHdlr::setMaxResponses,
                       SRC_POS);

      pkg->namesp_alias("EventRh");
    });
}

extern "C"
int Kbdresponsehdlr_Init(Tcl_Interp* interp)
{
GVX_TRACE("Kbdresponsehdlr_Init");

  return tcl::pkg::init
    (interp, "KbdResponseHdlr", "4.0",
     [](tcl::pkg* pkg) {
      tcl::def_creator<KbdResponseHdlr>(pkg);
      pkg->inherit_pkg("EventResponseHdlr");
      tcl::def_basic_type_cmds<KbdResponseHdlr>(pkg, SRC_POS);

      pkg->namesp_alias("KbdRh");
    });
}

extern "C"
int Nullresponsehdlr_Init(Tcl_Interp* interp)
{
GVX_TRACE("Nullresponsehdlr_Init");

  return tcl::pkg::init
    (interp, "NullResponseHdlr", "4.0",
     [](tcl::pkg* pkg) {
      tcl::def_creator<NullResponseHdlr>(pkg);
      pkg->inherit_pkg("ResponseHandler");
      tcl::def_basic_type_cmds<NullResponseHdlr>(pkg, SRC_POS);
      pkg->namesp_alias("NullRh");
    });
}

extern "C"
int Serialrh_Init(Tcl_Interp* interp)
{
GVX_TRACE("Serialrh_Init");

  return tcl::pkg::init
    (interp, "SerialRh", "4.0",
     [interp](tcl::pkg* pkg) {

      pkg->def( "SerialRh::SerialRh", "device=/dev/tty0p0",
                [interp](const char* device){
                  theEventSource.reset(new SerialEventSource(interp, device));
                }, SRC_POS );
      pkg->def( "SerialRh::SerialRh", "",
                [interp](){
                  theEventSource.reset(new SerialEventSource(interp, "/dev/tty0p0"));
                }, SRC_POS );

    });
}
