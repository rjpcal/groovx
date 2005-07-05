///////////////////////////////////////////////////////////////////////
//
// trialeventtcl.cc
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu May 22 14:37:17 2003
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

#ifndef GROOVX_VISX_TCLPKG_TRIALEVENT_CC_UTC20050628171009_DEFINED
#define GROOVX_VISX_TCLPKG_TRIALEVENT_CC_UTC20050628171009_DEFINED

#include "visx/tclpkg-trialevent.h"

#include "io/outputfile.h"

#include "nub/objfactory.h"

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/pkg.h"

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
    GVX_PKG_CREATE(pkg, interp, name, "4.$Revision$");
    nub::obj_factory::instance().register_creator(func, name);
    pkg->inherit_pkg("TrialEvent");

    GVX_PKG_RETURN(pkg);
  }

  void addEvents(nub::ref<MultiEvent> multi, tcl::list event_ids)
  {
    tcl::list::iterator<nub::ref<TrialEvent> >
      itr = event_ids.begin<nub::ref<TrialEvent> >(),
      end = event_ids.end<nub::ref<TrialEvent> >();

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

  GVX_PKG_CREATE(pkg, interp, "TrialEvent", "4.$Revision$");
  pkg->inherit_pkg("IO");
  tcl::def_basic_type_cmds<TrialEvent>(pkg, SRC_POS);

  pkg->def_get_set("delay", &TrialEvent::getDelay, &TrialEvent::setDelay, SRC_POS);

  GVX_PKG_FINISH(pkg);

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

  return GVX_PKG_STATUS;
}

extern "C"
int Nulltrialevent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Nulltrialevent_Init");

  GVX_PKG_CREATE(pkg, interp, "NullTrialEvent", "4.$Revision$");
  pkg->inherit_pkg("TrialEvent");
  tcl::def_creator<NullTrialEvent>(pkg);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Filewriteevent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Filewriteevent_Init");

  GVX_PKG_CREATE(pkg, interp, "FileWriteEvent", "4.$Revision$");
  pkg->inherit_pkg("TrialEvent");
  tcl::def_creator<FileWriteEvent>(pkg);

  pkg->def_get_set("file",
                   &FileWriteEvent::getFile,
                   &FileWriteEvent::setFile,
                   SRC_POS);
  pkg->def_get_set("byte",
                   &FileWriteEvent::getByte,
                   &FileWriteEvent::setByte,
                   SRC_POS);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Genericevent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Genericevent_Init");

  GVX_PKG_CREATE(pkg, interp, "GenericEvent", "4.$Revision$");
  pkg->inherit_pkg("TrialEvent");
  tcl::def_creator<GenericEvent>(pkg);

  pkg->def_getter("callback", &GenericEvent::getCallback, SRC_POS);
  pkg->def_setter("callback", &GenericEvent::setCallback, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Multievent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Multievent_Init");

  GVX_PKG_CREATE(pkg, interp, "MultiEvent", "4.$Revision$");
  pkg->inherit_pkg("TrialEvent");
  tcl::def_creator<MultiEvent>(pkg);

  pkg->def_getter("events", &MultiEvent::getEvents, SRC_POS);
  pkg->def("addEvent", "<this> event_id", &MultiEvent::addEvent, SRC_POS);
  pkg->def("addEvents", "<this> event_id(s)", &addEvents, SRC_POS);
  pkg->def("eraseEventAt", "<this> index", &MultiEvent::eraseEventAt, SRC_POS);
  pkg->def("clearEvents", "<this>", &MultiEvent::clearEvents, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_visx_tclpkg_trialevent_cc_utc20050628171009[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TCLPKG_TRIALEVENT_CC_UTC20050628171009_DEFINED