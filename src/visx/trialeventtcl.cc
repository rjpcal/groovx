///////////////////////////////////////////////////////////////////////
//
// trialeventtcl.cc
//
// Copyright (c) 2003-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu May 22 14:37:17 2003
// $Id$
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

#ifndef TRIALEVENTTCL_CC_DEFINED
#define TRIALEVENTTCL_CC_DEFINED

#include "io/outputfile.h"

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include "visx/trialevent.h"

#include <typeinfo>

#include "util/trace.h"

namespace
{
  typedef TrialEvent* (CreatorFunc)();

  Tcl::Pkg* addEventType(Tcl_Interp* interp,
                         CreatorFunc* func,
                         const char* name)
  {
    Tcl::Pkg* pkg = new Tcl::Pkg(interp, name, "$Revision$");
    Util::ObjFactory::theOne().registerCreatorFunc(func, name);
    pkg->inheritPkg("TrialEvent");

    return pkg;
  }

  template <class EventType>
  Tcl::Pkg* initEventType(Tcl_Interp* interp)
  {
    Tcl::Pkg* pkg = new Tcl::Pkg(interp,
                                 demangle_cstr(typeid(EventType).name()),
                                 "$Revision$");
    Tcl::defCreator<EventType>(pkg);
    pkg->inheritPkg("TrialEvent");

    return pkg;
  }

  void addEvents(Util::Ref<MultiEvent> multi, Tcl::List event_ids)
  {
    Tcl::List::Iterator<Util::Ref<TrialEvent> >
      itr = event_ids.begin<Util::Ref<TrialEvent> >(),
      end = event_ids.end<Util::Ref<TrialEvent> >();

    while (itr != end)
      {
        multi->addEvent(*itr);
        ++itr;
      }
  }
}

extern "C"
int Trialevent_Init(Tcl_Interp* interp)
{
DOTRACE("Trialevent_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "TrialEvent", "$Revision$");
  pkg->inheritPkg("IO");
  Tcl::defGenericObjCmds<TrialEvent>(pkg);

  pkg->defAttrib("delay", &TrialEvent::getDelay, &TrialEvent::setDelay);

  addEventType(interp, &makeAbortTrialEvent, "AbortTrialEvent");
  addEventType(interp, &makeDrawEvent, "DrawEvent");
  addEventType(interp, &makeRenderEvent, "RenderEvent");
  addEventType(interp, &makeUndrawEvent, "UndrawEvent");
  addEventType(interp, &makeEndTrialEvent, "EndTrialEvent");
  addEventType(interp, &makeNextNodeEvent, "NextNodeEvent");
  addEventType(interp, &makeAllowResponsesEvent, "AllowResponsesEvent");
  addEventType(interp, &makeDenyResponsesEvent, "DenyResponsesEvent");
  addEventType(interp, &makeSwapBuffersEvent, "SwapBuffersEvent");
  addEventType(interp, &makeRenderBackEvent, "RenderBackEvent");
  addEventType(interp, &makeRenderFrontEvent, "RenderFrontEvent");
  addEventType(interp, &makeClearBufferEvent, "ClearBufferEvent");
  addEventType(interp, &makeFinishDrawingEvent, "FinishDrawingEvent");

  return pkg->initStatus();
}

extern "C"
int Filewriteevent_Init(Tcl_Interp* interp)
{
DOTRACE("Filewriteevent_Init");

  Tcl::Pkg* pkg = initEventType<FileWriteEvent>(interp);

  pkg->defAttrib("file",
                 &FileWriteEvent::getFile,
                 &FileWriteEvent::setFile);
  pkg->defAttrib("byte",
                 &FileWriteEvent::getByte,
                 &FileWriteEvent::setByte);

  return pkg->initStatus();
}

extern "C"
int Genericevent_Init(Tcl_Interp* interp)
{
DOTRACE("Genericevent_Init");

  Tcl::Pkg* pkg = initEventType<GenericEvent>(interp);

  pkg->defGetter("callback", &GenericEvent::getCallback);
  pkg->defSetter("callback", &GenericEvent::setCallback);

  return pkg->initStatus();
}

extern "C"
int Multievent_Init(Tcl_Interp* interp)
{
DOTRACE("Multievent_Init");

  Tcl::Pkg* pkg = initEventType<MultiEvent>(interp);

  pkg->defGetter("events", &MultiEvent::getEvents);
  pkg->def("addEvent", "<this> event_id", &MultiEvent::addEvent);
  pkg->def("addEvents", "<this> event_id(s)", &addEvents);
  pkg->def("eraseEventAt", "<this> index", &MultiEvent::eraseEventAt);
  pkg->def("clearEvents", "<this>", &MultiEvent::clearEvents);

  return pkg->initStatus();
}

static const char vcid_trialeventtcl_cc[] = "$Header$";
#endif // !TRIALEVENTTCL_CC_DEFINED
