///////////////////////////////////////////////////////////////////////
//
// rhtcl.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun  9 20:39:46 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef RHTCL_CC_DEFINED
#define RHTCL_CC_DEFINED

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"
#include "util/pointers.h"
#include "util/serialport.h"
#include "util/strings.h"

#include "visx/eventresponsehdlr.h"
#include "visx/responsehandler.h"
#include "visx/kbdresponsehdlr.h"
#include "visx/nullresponsehdlr.h"

#include <tk.h>
#include <X11/Xlib.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER;

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
// Package init procedures
//
//--------------------------------------------------------------------

extern "C"
int Responsehandler_Init(Tcl_Interp* interp)
{
DOTRACE("Responsehandler_Init");

  PKG_CREATE(interp, "ResponseHandler", "$Revision$");
  pkg->inheritPkg("IO");
  Tcl::defGenericObjCmds<ResponseHandler>(pkg);
  pkg->namespaceAlias("Rh");

  PKG_RETURN;
}

extern "C"
int Eventresponsehdlr_Init(Tcl_Interp* interp)
{
DOTRACE("Eventresponsehdlr_Init");

  PKG_CREATE(interp, "EventResponseHdlr", "$Revision$");
  Tcl::defCreator<EventResponseHdlr>(pkg);
  pkg->inheritPkg("ResponseHandler");
  Tcl::defTracing(pkg, EventResponseHdlr::tracer);

  Tcl::defGenericObjCmds<EventResponseHdlr>(pkg);

  pkg->defAction("abortInvalidResponses",
                 &EventResponseHdlr::abortInvalidResponses);
  pkg->defAction("ignoreInvalidResponses",
                 &EventResponseHdlr::ignoreInvalidResponses);
  pkg->defAttrib("useFeedback",
                 &EventResponseHdlr::getUseFeedback,
                 &EventResponseHdlr::setUseFeedback);
  pkg->defSetter("inputResponseMap",
                 &EventResponseHdlr::setInputResponseMap);
  pkg->defAttrib("feedbackMap",
                 &EventResponseHdlr::getFeedbackMap,
                 &EventResponseHdlr::setFeedbackMap);
  pkg->defAttrib("eventSequence",
                 &EventResponseHdlr::getEventSequence,
                 &EventResponseHdlr::setEventSequence);
  pkg->defAttrib("bindingSubstitution",
                 &EventResponseHdlr::getBindingSubstitution,
                 &EventResponseHdlr::setBindingSubstitution);
  pkg->def("responseProc", "", &EventResponseHdlr::getResponseProc);
  pkg->def("responseProc", "args body", &EventResponseHdlr::setResponseProc);
  pkg->defAttrib("maxResponses",
                 &EventResponseHdlr::getMaxResponses,
                 &EventResponseHdlr::setMaxResponses);

  pkg->namespaceAlias("EventRh");

  PKG_RETURN;
}

extern "C"
int Kbdresponsehdlr_Init(Tcl_Interp* interp)
{
DOTRACE("Kbdresponsehdlr_Init");

  PKG_CREATE(interp, "KbdResponseHdlr", "$Revision$");
  Tcl::defCreator<KbdResponseHdlr>(pkg);
  pkg->inheritPkg("EventResponseHdlr");
  Tcl::defGenericObjCmds<KbdResponseHdlr>(pkg);

  pkg->namespaceAlias("KbdRh");

  PKG_RETURN;
}

extern "C"
int Nullresponsehdlr_Init(Tcl_Interp* interp)
{
DOTRACE("Nullresponsehdlr_Init");

  PKG_CREATE(interp, "NullResponseHdlr", "$Revision$");
  Tcl::defCreator<NullResponseHdlr>(pkg);
  pkg->inheritPkg("ResponseHandler");
  Tcl::defGenericObjCmds<NullResponseHdlr>(pkg);
  pkg->namespaceAlias("NullRh");

  PKG_RETURN;
}

extern "C"
int Serialrh_Init(Tcl_Interp* interp)
{
DOTRACE("Serialrh_Init");

  PKG_CREATE(interp, "SerialRh", "$Revision$");
  pkg->def( "SerialRh::SerialRh", "device=/dev/tty0p0",
            Util::bindFirst(&startSerial, interp) );
  pkg->def( "SerialRh::SerialRh", "",
            Util::bindLast(Util::bindFirst(&startSerial, interp),
                           "/dev/tty0p0") );

  PKG_RETURN;
}

static const char vcid_rhtcl_cc[] = "$Header$";
#endif // !RHTCL_CC_DEFINED
