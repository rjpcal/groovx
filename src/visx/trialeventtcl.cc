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

  int addEventType(Tcl_Interp* interp,
                   CreatorFunc* func,
                   const char* name)
  {
    PKG_CREATE(interp, name, "$Revision$");
    Util::ObjFactory::theOne().registerCreatorFunc(func, name);
    pkg->inheritPkg("TrialEvent");

    PKG_RETURN;
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

  PKG_CREATE(interp, "TrialEvent", "$Revision$");
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

  PKG_RETURN;
}

extern "C"
int Nulltrialevent_Init(Tcl_Interp* interp)
{
DOTRACE("Nulltrialevent_Init");

  PKG_CREATE(interp, "NullTrialEvent", "$Revision$");
  pkg->inheritPkg("TrialEvent");
  Tcl::defCreator<NullTrialEvent>(pkg);

  PKG_RETURN;
}

extern "C"
int Filewriteevent_Init(Tcl_Interp* interp)
{
DOTRACE("Filewriteevent_Init");

  PKG_CREATE(interp, "FileWriteEvent", "$Revision$");
  pkg->inheritPkg("TrialEvent");
  Tcl::defCreator<FileWriteEvent>(pkg);

  pkg->defAttrib("file",
                 &FileWriteEvent::getFile,
                 &FileWriteEvent::setFile);
  pkg->defAttrib("byte",
                 &FileWriteEvent::getByte,
                 &FileWriteEvent::setByte);

  PKG_RETURN;
}

extern "C"
int Genericevent_Init(Tcl_Interp* interp)
{
DOTRACE("Genericevent_Init");

  PKG_CREATE(interp, "GenericEvent", "$Revision$");
  pkg->inheritPkg("TrialEvent");
  Tcl::defCreator<GenericEvent>(pkg);

  pkg->defGetter("callback", &GenericEvent::getCallback);
  pkg->defSetter("callback", &GenericEvent::setCallback);

  PKG_RETURN;
}

extern "C"
int Multievent_Init(Tcl_Interp* interp)
{
DOTRACE("Multievent_Init");

  PKG_CREATE(interp, "MultiEvent", "$Revision$");
  pkg->inheritPkg("TrialEvent");
  Tcl::defCreator<MultiEvent>(pkg);

  pkg->defGetter("events", &MultiEvent::getEvents);
  pkg->def("addEvent", "<this> event_id", &MultiEvent::addEvent);
  pkg->def("addEvents", "<this> event_id(s)", &addEvents);
  pkg->def("eraseEventAt", "<this> index", &MultiEvent::eraseEventAt);
  pkg->def("clearEvents", "<this>", &MultiEvent::clearEvents);

  PKG_RETURN;
}

static const char vcid_trialeventtcl_cc[] = "$Header$";
#endif // !TRIALEVENTTCL_CC_DEFINED