///////////////////////////////////////////////////////////////////////
//
// expttcl.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Mar  8 03:18:40 1999
// commit: $Id$
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

#ifndef EXPTTCL_CC_DEFINED
#define EXPTTCL_CC_DEFINED

#include "gfx/toglet.h"

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tracertcl.h"

#include "util/ref.h"
#include "util/strings.h"

#include "visx/exptdriver.h"

#include "util/trace.h"

namespace
{
  // Creates the necessary screen bindings for start, pause, and quit,
  // then begins the current trial (probably the first trial) of the
  // current Expt. Record the time when the experiment
  // began. edBeginExpt is called, which displays a trial, and
  // generates the timer callbacks associated with a trial.
  void beginExpt(Util::Ref<ExptDriver> xp)
  {
    Util::SoftRef<Toglet> w = xp->getWidget();

    // Create the begin key binding
    w->bind("<Control-KeyPress-b>",
            fstring("-> ", w.id(), " takeFocus; -> ", xp.id(), " begin"));

    // Create the quit key binding
    w->bind("<Control-KeyPress-q>",
            fstring("-> ", xp.id(), " halt; -> ", xp.id(), " storeData; exit"));

    // Create the quit-without-save key binding
    w->bind("<Control-Alt-KeyPress-x>", "exit");

    // Create the save key binding
    w->bind("<Control-KeyPress-s>", fstring("-> ", xp.id(), " storeData"));

    // Create the stop key binding
    w->bind("<Control-KeyPress-c>", fstring("-> ", xp.id(), " stop"));

    // Create the reset key binding
    w->bind("<Control-KeyPress-r>", fstring("-> ", xp.id(), " reset"));

#if 0
    // Create the pause key binding
    w->bind("<KeyPress-p>", fstring("-> ", xp.id(), " pause"));
#endif

    // Force the focus to the widget
    w->takeFocus();

    xp->edBeginExpt();
  }

  void waitStartKey(Util::Ref<ExptDriver> xp, const char* event)
  {
    Util::SoftRef<Toglet> w = xp->getWidget();

    // This script does two things:

    // (1) destroy the binding that generated this callback (so that we
    // don't accidentally "start" the experiment twice due to a double
    // keypress)
    // (2) actually start the experimeent
    const fstring script("-> ", w.id(), " bind ", event, " {}; "
                         "-> ", xp.id(), " begin");

    w->bind(event, script);
    w->takeFocus();
  }

  void fakePause(Util::Ref<ExptDriver>) {}
}

extern "C"
int Exptdriver_Init(Tcl_Interp* interp)
{
DOTRACE("Exptdriver_Init");

  PKG_CREATE(interp, "ExptDriver", "$Revision$");
  pkg->inheritPkg("ElementContainer");
  Tcl::defCreator<ExptDriver>(pkg);
  Tcl::defGenericObjCmds<ExptDriver>(pkg, SRC_POS);

  Tcl::defTracing(pkg, ExptDriver::tracer);

  pkg->def( "begin", "expt_id", &beginExpt, SRC_POS );
  pkg->def( "waitStartKey", "expt_id <event>", &waitStartKey, SRC_POS );

  pkg->defAttrib("autosaveFile",
                 &ExptDriver::getAutosaveFile,
                 &ExptDriver::setAutosaveFile,
                 SRC_POS);
  pkg->defAttrib("autosavePeriod",
                 &ExptDriver::getAutosavePeriod,
                 &ExptDriver::setAutosavePeriod,
                 SRC_POS);
  pkg->defAction("claimLogFile", &ExptDriver::claimLogFile, SRC_POS);
  pkg->defAttrib("filePrefix",
                 &ExptDriver::getFilePrefix,
                 &ExptDriver::setFilePrefix,
                 SRC_POS);
  pkg->defGetter("infoLog", &ExptDriver::getInfoLog, SRC_POS);
  pkg->def("pause", "expt_id", &fakePause, SRC_POS);
  pkg->defAction("storeData", &ExptDriver::storeData, SRC_POS);
  // FIXME this conflicts with Element::widget
  pkg->defAttrib("widget", &ExptDriver::getWidget, &ExptDriver::setWidget, SRC_POS);
  pkg->defGetter("doWhenComplete", &ExptDriver::getDoWhenComplete, SRC_POS);
  pkg->defSetter("doWhenComplete", &ExptDriver::setDoWhenComplete, SRC_POS);

  PKG_RETURN;
}

static const char vcid_expttcl_cc[] = "$Header$";
#endif // !EXPTTCL_CC_DEFINED
