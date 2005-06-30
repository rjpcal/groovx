///////////////////////////////////////////////////////////////////////
//
// rhtcl.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jun  9 20:39:46 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_TCLPKG_RH_CC_UTC20050628171008_DEFINED
#define GROOVX_VISX_TCLPKG_RH_CC_UTC20050628171008_DEFINED

#include "visx/tclpkg-rh.h"

#include "visx/tclpkg-rh.h"

#include "nub/objfactory.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"
#include "tcl/tracertcl.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sharedptr.h"
#include "rutz/serialport.h"

#include "visx/eventresponsehdlr.h"
#include "visx/responsehandler.h"
#include "visx/kbdresponsehdlr.h"
#include "visx/nullresponsehdlr.h"

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

  rutz::shared_ptr<SerialEventSource> theEventSource;

  void startSerial(Tcl_Interp* interp, const char* device)
  {
    theEventSource.reset(new SerialEventSource(interp, device));
  }
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

  GVX_PKG_CREATE(pkg, interp, "ResponseHandler", "4.$Revision$");
  pkg->inherit_pkg("IO");
  tcl::def_basic_type_cmds<ResponseHandler>(pkg, SRC_POS);
  pkg->namesp_alias("Rh");

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Eventresponsehdlr_Init(Tcl_Interp* interp)
{
GVX_TRACE("Eventresponsehdlr_Init");

  GVX_PKG_CREATE(pkg, interp, "EventResponseHdlr", "4.$Revision$");
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

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Kbdresponsehdlr_Init(Tcl_Interp* interp)
{
GVX_TRACE("Kbdresponsehdlr_Init");

  GVX_PKG_CREATE(pkg, interp, "KbdResponseHdlr", "4.$Revision$");
  tcl::def_creator<KbdResponseHdlr>(pkg);
  pkg->inherit_pkg("EventResponseHdlr");
  tcl::def_basic_type_cmds<KbdResponseHdlr>(pkg, SRC_POS);

  pkg->namesp_alias("KbdRh");

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Nullresponsehdlr_Init(Tcl_Interp* interp)
{
GVX_TRACE("Nullresponsehdlr_Init");

  GVX_PKG_CREATE(pkg, interp, "NullResponseHdlr", "4.$Revision$");
  tcl::def_creator<NullResponseHdlr>(pkg);
  pkg->inherit_pkg("ResponseHandler");
  tcl::def_basic_type_cmds<NullResponseHdlr>(pkg, SRC_POS);
  pkg->namesp_alias("NullRh");

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Serialrh_Init(Tcl_Interp* interp)
{
GVX_TRACE("Serialrh_Init");

  GVX_PKG_CREATE(pkg, interp, "SerialRh", "4.$Revision$");
  pkg->def( "SerialRh::SerialRh", "device=/dev/tty0p0",
            rutz::bind_first(&startSerial, interp),
            SRC_POS );
  pkg->def( "SerialRh::SerialRh", "",
            rutz::bind_last(rutz::bind_first(&startSerial, interp),
                            "/dev/tty0p0"),
            SRC_POS );

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_visx_tclpkg_rh_cc_utc20050628171008[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TCLPKG_RH_CC_UTC20050628171008_DEFINED
