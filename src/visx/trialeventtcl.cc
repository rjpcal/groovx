///////////////////////////////////////////////////////////////////////
//
// trialeventtcl.cc
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu May 22 14:37:17 2003
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_TRIALEVENTTCL_CC_UTC20050626084016_DEFINED
#define GROOVX_VISX_TRIALEVENTTCL_CC_UTC20050626084016_DEFINED

#include "visx/trialeventtcl.h"

#include "io/outputfile.h"

#include "nub/objfactory.h"

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "visx/trialevent.h"

#include <typeinfo>

#include "rutz/trace.h"

namespace
{
  typedef TrialEvent* (CreatorFunc)();

  int addEventType(Tcl_Interp* interp,
                   CreatorFunc* func,
                   const char* name) throw()
  {
    PKG_CREATE(interp, name, "4.$Revision$");
    Nub::ObjFactory::theOne().register_creator(func, name);
    pkg->inheritPkg("TrialEvent");

    PKG_RETURN;
  }

  void addEvents(Nub::Ref<MultiEvent> multi, Tcl::List event_ids)
  {
    Tcl::List::Iterator<Nub::Ref<TrialEvent> >
      itr = event_ids.begin<Nub::Ref<TrialEvent> >(),
      end = event_ids.end<Nub::Ref<TrialEvent> >();

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
GVX_TRACE("Trialevent_Init");

  PKG_CREATE(interp, "TrialEvent", "4.$Revision$");
  pkg->inheritPkg("IO");
  Tcl::defGenericObjCmds<TrialEvent>(pkg, SRC_POS);

  pkg->defAttrib("delay", &TrialEvent::getDelay, &TrialEvent::setDelay, SRC_POS);

  PKG_FINISH;

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

  return PKG_STATUS;
}

extern "C"
int Nulltrialevent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Nulltrialevent_Init");

  PKG_CREATE(interp, "NullTrialEvent", "4.$Revision$");
  pkg->inheritPkg("TrialEvent");
  Tcl::defCreator<NullTrialEvent>(pkg);

  PKG_RETURN;
}

extern "C"
int Filewriteevent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Filewriteevent_Init");

  PKG_CREATE(interp, "FileWriteEvent", "4.$Revision$");
  pkg->inheritPkg("TrialEvent");
  Tcl::defCreator<FileWriteEvent>(pkg);

  pkg->defAttrib("file",
                 &FileWriteEvent::getFile,
                 &FileWriteEvent::setFile,
                 SRC_POS);
  pkg->defAttrib("byte",
                 &FileWriteEvent::getByte,
                 &FileWriteEvent::setByte,
                 SRC_POS);

  PKG_RETURN;
}

extern "C"
int Genericevent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Genericevent_Init");

  PKG_CREATE(interp, "GenericEvent", "4.$Revision$");
  pkg->inheritPkg("TrialEvent");
  Tcl::defCreator<GenericEvent>(pkg);

  pkg->defGetter("callback", &GenericEvent::getCallback, SRC_POS);
  pkg->defSetter("callback", &GenericEvent::setCallback, SRC_POS);

  PKG_RETURN;
}

extern "C"
int Multievent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Multievent_Init");

  PKG_CREATE(interp, "MultiEvent", "4.$Revision$");
  pkg->inheritPkg("TrialEvent");
  Tcl::defCreator<MultiEvent>(pkg);

  pkg->defGetter("events", &MultiEvent::getEvents, SRC_POS);
  pkg->def("addEvent", "<this> event_id", &MultiEvent::addEvent, SRC_POS);
  pkg->def("addEvents", "<this> event_id(s)", &addEvents, SRC_POS);
  pkg->def("eraseEventAt", "<this> index", &MultiEvent::eraseEventAt, SRC_POS);
  pkg->def("clearEvents", "<this>", &MultiEvent::clearEvents, SRC_POS);

  PKG_RETURN;
}

static const char vcid_groovx_visx_trialeventtcl_cc_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TRIALEVENTTCL_CC_UTC20050626084016_DEFINED
