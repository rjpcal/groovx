/** @file visx/tclpkg-trialevent.cc tcl interface package for class
    TrialEvent and derivatives */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu May 22 14:37:17 2003
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

  /// return 1 if error, 0 if ok
  int addEventType(Tcl_Interp* interp,
                   CreatorFunc* func,
                   const char* name) noexcept
  {
    nub::obj_factory::instance().register_creator(func, name);
    return tcl::pkg::STATUS_OK == tcl::pkg::init
      (interp, name, "4.0",
       [](tcl::pkg* pkg) {
        pkg->inherit_pkg("TrialEvent");
      })
      ? 0 : 1;
  }

  void addEvents(nub::ref<MultiEvent> multi, tcl::list event_ids)
  {
    for (const auto& evref: event_ids.view_of<nub::ref<TrialEvent>>())
      multi->addEvent(evref);
  }
}

extern "C"
int Trialevent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Trialevent_Init");

  int errors = 0;
  errors += tcl::pkg::STATUS_OK != tcl::pkg::init
    (interp, "TrialEvent", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("io");
      tcl::def_basic_type_cmds<TrialEvent>(pkg, SRC_POS);

      pkg->def_get_set("delay", &TrialEvent::getDelay, &TrialEvent::setDelay, SRC_POS);
    });

  errors += addEventType(interp, &makeAbortTrialEvent, "AbortTrialEvent");
  errors += addEventType(interp, &makeDrawEvent, "DrawEvent");
  errors += addEventType(interp, &makeRenderEvent, "RenderEvent");
  errors += addEventType(interp, &makeUndrawEvent, "UndrawEvent");
  errors += addEventType(interp, &makeEndTrialEvent, "EndTrialEvent");
  errors += addEventType(interp, &makeNextNodeEvent, "NextNodeEvent");
  errors += addEventType(interp, &makeAllowResponsesEvent, "AllowResponsesEvent");
  errors += addEventType(interp, &makeDenyResponsesEvent, "DenyResponsesEvent");
  errors += addEventType(interp, &makeSwapBuffersEvent, "SwapBuffersEvent");
  errors += addEventType(interp, &makeRenderBackEvent, "RenderBackEvent");
  errors += addEventType(interp, &makeRenderFrontEvent, "RenderFrontEvent");
  errors += addEventType(interp, &makeClearBufferEvent, "ClearBufferEvent");
  errors += addEventType(interp, &makeFinishDrawingEvent, "FinishDrawingEvent");

  return errors ? tcl::pkg::STATUS_ERR : tcl::pkg::STATUS_OK;
}

extern "C"
int Nulltrialevent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Nulltrialevent_Init");

  return tcl::pkg::init
    (interp, "NullTrialEvent", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("TrialEvent");
      tcl::def_creator<NullTrialEvent>(pkg);
    });
}

extern "C"
int Filewriteevent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Filewriteevent_Init");

  return tcl::pkg::init
    (interp, "FileWriteEvent", "4.0",
     [](tcl::pkg* pkg) {
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
    });
}

extern "C"
int Genericevent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Genericevent_Init");

  return tcl::pkg::init
    (interp, "GenericEvent", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("TrialEvent");
      tcl::def_creator<GenericEvent>(pkg);

      pkg->def_getter("callback", &GenericEvent::getCallback, SRC_POS);
      pkg->def_setter("callback", &GenericEvent::setCallback, SRC_POS);
    });
}

extern "C"
int Multievent_Init(Tcl_Interp* interp)
{
GVX_TRACE("Multievent_Init");

  return tcl::pkg::init
    (interp, "MultiEvent", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("TrialEvent");
      tcl::def_creator<MultiEvent>(pkg);

      pkg->def_getter("events", &MultiEvent::getEvents, SRC_POS);
      pkg->def("addEvent", "<this> event_id", &MultiEvent::addEvent, SRC_POS);
      pkg->def("addEvents", "<this> event_id(s)", &addEvents, SRC_POS);
      pkg->def("eraseEventAt", "<this> index", &MultiEvent::eraseEventAt, SRC_POS);
      pkg->def("clearEvents", "<this>", &MultiEvent::clearEvents, SRC_POS);
    });
}
