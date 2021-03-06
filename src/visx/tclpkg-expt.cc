/** @file visx/tclpkg-expt.cc tcl interface package for class ExptDriver */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Mar  8 03:18:40 1999
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

#include "visx/tclpkg-expt.h"

#include "nub/ref.h"

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/pkg.h"
#include "tcl/tracertcl.h"

#include "tcl-gfx/toglet.h"

#include "rutz/fstring.h"
#include "rutz/sfmt.h"

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
            rutz::sfmt("-> %lu takeFocus; -> %lu begin",
                       w.id(), xp.id()));

    // Create the quit key binding
    w->bind("<Control-KeyPress-q>",
            rutz::sfmt("-> %lu halt; -> %lu storeData; exit",
                       xp.id(), xp.id()));

    // Create the quit-without-save key binding
    w->bind("<Control-Alt-KeyPress-x>", "exit");

    // Create the save key binding
    w->bind("<Control-KeyPress-s>", rutz::sfmt("-> %lu storeData", xp.id()));

    // Create the stop key binding
    w->bind("<Control-KeyPress-c>", rutz::sfmt("-> %lu stop", xp.id()));

    // Create the reset key binding
    w->bind("<Control-KeyPress-r>", rutz::sfmt("-> %lu reset", xp.id()));

#if 0
    // Create the pause key binding
    w->bind("<KeyPress-p>", rutz::sfmt("-> %lu pause", xp.id()));
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
    const fstring script =
      rutz::sfmt("-> %lu bind %s {}; -> %lu begin",
                 w.id(), event, xp.id());

    w->bind(event, script);
    w->takeFocus();
  }

  void fakePause(nub::ref<ExptDriver>) {}
}

extern "C"
int Exptdriver_Init(Tcl_Interp* interp)
{
GVX_TRACE("Exptdriver_Init");

  return tcl::pkg::init
    (interp, "ExptDriver", "4.0",
     [](tcl::pkg* pkg) {
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
    });
}
