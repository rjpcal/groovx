///////////////////////////////////////////////////////////////////////
//
// expttcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Mar  8 03:18:40 1999
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
  void beginExpt(Util::Ref<ExptDriver> expt)
  {
    Util::SoftRef<Toglet> widget = expt->getWidget();

    // Create the begin key binding
    widget->bind("<Control-KeyPress-b>", "Togl::takeFocus; Expt::begin");

    // Create the quit key binding
    widget->bind("<Control-KeyPress-q>", "Expt::storeData; exit");

    // Create the quit-without-save key binding
    widget->bind("<Control-Alt-KeyPress-x>", "exit");

    // Create the save key binding
    widget->bind("<Control-KeyPress-s>", "Expt::storeData");

    // Create the stop key binding
    widget->bind("<Control-KeyPress-c>", "Expt::stop");

    // Create the reset key binding
    widget->bind("<Control-KeyPress-r>", "Expt::reset");

    // Create the pause key binding
    widget->bind("<KeyPress-p>", "Expt::pause");

    // Destroy the experiment start key binding
    widget->bind("<KeyPress-s>", "");

    // Force the focus to the widget
    widget->takeFocus();

    expt->edBeginExpt();
  }

  void setStartCommand(Ref<ExptDriver> expt, const char* command)
  {
    Util::SoftRef<Toglet> widget = expt->getWidget();

    widget->bind("<KeyPress-s>", command);
    widget->takeFocus();
  }
}

extern "C"
int Exptdriver_Init(Tcl_Interp* interp)
{
DOTRACE("Exptdriver_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "ExptDriver", "$Revision$");
  pkg->inheritPkg("IO");
  Tcl::defCreator<ExptDriver>(pkg, "Expt");
  Tcl::defGenericObjCmds<ExptDriver>(pkg);

  Tcl::defTracing(pkg, ExptDriver::tracer);

  pkg->def( "begin", "expt_id", &beginExpt );
  pkg->def( "setStartCommand", "expt_id command", &setStartCommand );

  pkg->def("addBlock", "expt_id block_id",
           Util::bindLast(Util::memFunc(&ExptDriver::addElement), 1));
  pkg->defAttrib("autosaveFile",
                 &ExptDriver::getAutosaveFile, &ExptDriver::setAutosaveFile);
  pkg->defAttrib("autosavePeriod",
                 &ExptDriver::getAutosavePeriod,
                 &ExptDriver::setAutosavePeriod);
  pkg->defGetter("blocks", &ExptDriver::getElements);
  pkg->defAction("clear", &ExptDriver::clearElements);
  pkg->defGetter("currentBlock", &ExptDriver::currentElement);
  pkg->defGetter("infoLog", &ExptDriver::getInfoLog);
  pkg->defAction( "pause", &ExptDriver::pause );
  pkg->defAction("reset", &ExptDriver::vxReset);
  pkg->defAction("stop", &ExptDriver::vxHalt);
  pkg->defAction("storeData", &ExptDriver::storeData);
  pkg->defAttrib("widget", &ExptDriver::getWidget, &ExptDriver::setWidget);
  pkg->defGetter("doWhenComplete", &ExptDriver::getDoWhenComplete);
  pkg->defSetter("doWhenComplete", &ExptDriver::setDoWhenComplete);

  return pkg->initStatus();
}

static const char vcid_expttcl_cc[] = "$Header$";
#endif // !EXPTTCL_CC_DEFINED
