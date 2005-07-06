/** @file visx/tclpkg-expt.cc tcl interface package for class ExptDriver */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Mar  8 03:18:40 1999
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

#ifndef GROOVX_VISX_TCLPKG_EXPT_CC_UTC20050628171008_DEFINED
#define GROOVX_VISX_TCLPKG_EXPT_CC_UTC20050628171008_DEFINED

#include "visx/tclpkg-expt.h"

#include "gfx/toglet.h"

#include "nub/ref.h"

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/pkg.h"
#include "tcl/tracertcl.h"

#include "rutz/fstring.h"

#include "visx/exptdriver.h"

#include "rutz/trace.h"

using rutz::fstring;

namespace
{
  // Creates the necessary screen bindings for start, pause, and quit,
  // then begins the current trial (probably the first trial) of the
  // current Expt. Record the time when the experiment
  // began. edBeginExpt is called, which displays a trial, and
  // generates the timer callbacks associated with a trial.
  void beginExpt(nub::ref<ExptDriver> xp)
  {
    nub::soft_ref<Toglet> w = xp->getWidget();

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

  void waitStartKey(nub::ref<ExptDriver> xp, const char* event)
  {
    nub::soft_ref<Toglet> w = xp->getWidget();

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

  void fakePause(nub::ref<ExptDriver>) {}
}

extern "C"
int Exptdriver_Init(Tcl_Interp* interp)
{
GVX_TRACE("Exptdriver_Init");

  GVX_PKG_CREATE(pkg, interp, "ExptDriver", "4.$Revision$");
  pkg->inherit_pkg("ElementContainer");
  tcl::def_creator<ExptDriver>(pkg);
  tcl::def_basic_type_cmds<ExptDriver>(pkg, SRC_POS);

  tcl::def_tracing(pkg, ExptDriver::tracer);

  pkg->def( "begin", "expt_id", &beginExpt, SRC_POS );
  pkg->def( "waitStartKey", "expt_id <event>", &waitStartKey, SRC_POS );

  pkg->def_get_set("autosaveFile",
                   &ExptDriver::getAutosaveFile,
                   &ExptDriver::setAutosaveFile,
                   SRC_POS);
  pkg->def_get_set("autosavePeriod",
                   &ExptDriver::getAutosavePeriod,
                   &ExptDriver::setAutosavePeriod,
                   SRC_POS);
  pkg->def_action("claimLogFile", &ExptDriver::claimLogFile, SRC_POS);
  pkg->def_get_set("filePrefix",
                   &ExptDriver::getFilePrefix,
                   &ExptDriver::setFilePrefix,
                   SRC_POS);
  pkg->def_getter("infoLog", &ExptDriver::getInfoLog, SRC_POS);
  pkg->def("pause", "expt_id", &fakePause, SRC_POS);
  pkg->def_action("storeData", &ExptDriver::storeData, SRC_POS);
  // FIXME this conflicts with Element::widget
  pkg->def_get_set("widget", &ExptDriver::getWidget, &ExptDriver::setWidget, SRC_POS);
  pkg->def_getter("doWhenComplete", &ExptDriver::getDoWhenComplete, SRC_POS);
  pkg->def_setter("doWhenComplete", &ExptDriver::setDoWhenComplete, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_visx_tclpkg_expt_cc_utc20050628171008[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TCLPKG_EXPT_CC_UTC20050628171008_DEFINED
