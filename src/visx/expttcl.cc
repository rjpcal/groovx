///////////////////////////////////////////////////////////////////////
//
// expttcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  8 03:18:40 1999
// written: Wed Dec  4 18:10:03 2002
// $Id$
//
// This file defines the procedures that provide the Tcl interface to
// the ExptDriver class, and defines several Tcl variables that are
// provided within the Tcl Expt:: namespace.
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_CC_DEFINED
#define EXPTTCL_CC_DEFINED

#include "visx/block.h"
#include "visx/exptdriver.h"

#include "system/system.h"

#include "tcl/iotcl.h"
#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/tclcode.h"
#include "tcl/tclpkg.h"
#include "tcl/tclsafeinterp.h"
#include "tcl/toglet.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"
#include "util/ref.h"
#include "util/strings.h"

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Expt Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace ExptTcl
{
  class ExpPkg;

  SoftRef<ExptDriver> theExptDriver;

  void setCurrentExpt(Ref<ExptDriver> expt)
  {
    theExptDriver = expt;
  }

  Ref<ExptDriver> getCurrentExpt()
  {
    return theExptDriver;
  }

  // Creates the necessary screen bindings for start, pause, and quit,
  // then begins the current trial (probably the first trial) of the
  // current Expt. Record the time when the experiment
  // began. edBeginExpt is called, which displays a trial, and
  // generates the timer callbacks associated with a trial.
  void begin(Ref<ExptDriver> expt)
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

  Tcl::Code thePauseMsgCmd
  (Tcl::toTcl("tk_messageBox -default ok -icon info "
              "-title \"Pause\" -type ok "
              "-message \"Experiment paused. Click OK to continue.\";\n"),
   Tcl::Code::THROW_EXCEPTION);

  // Tell the ExptDriver to halt the experiment, then pop up a pause
  // window. When the user dismisses the window, the experiment will
  // resume.
  void pause(Ref<ExptDriver> ed)
  {
    ed->vxHalt();

    ed->addLogInfo("Experiment paused.");

    thePauseMsgCmd.invoke(ed->getInterp());

    Tcl::Interp::clearEventQueue();

    Util::SoftRef<Toglet> widget = ed->getWidget();
    widget->fullClearscreen();
    widget->fullClearscreen();

    System::theSystem().sleep(2);

    widget->fullClearscreen();
    widget->fullClearscreen();

    Tcl::Interp::clearEventQueue();

    ed->addLogInfo("Resuming experiment.");

    ed->edResumeExpt();
  }

  void setStartCommand(Ref<ExptDriver> expt, const char* command)
  {
    Util::SoftRef<Toglet> widget = expt->getWidget();

    widget->bind("<KeyPress-s>", command);
    widget->takeFocus();
  }
}

//---------------------------------------------------------------------
//
// ExpPkg definition
//
//---------------------------------------------------------------------

class ExptTcl::ExpPkg : public Tcl::Pkg
{
public:
  ExpPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Exp", "$Revision$")
  {
    theExptDriver = Ref<ExptDriver>(ExptDriver::make());

    Tcl::defGenericObjCmds<ExptDriver>(this);

    Tcl::defIoCommands(this);

    Tcl::defTracing(this, ExptDriver::tracer);

    def( "currentExp", "expt_id", &ExptTcl::setCurrentExpt );
    def( "currentExp", 0, &ExptTcl::getCurrentExpt );

    def( "begin", "expt_id", ExptTcl::begin );
    def( "pause", "expt_id", &ExptTcl::pause );
    def( "setStartCommand", "expt_id command", ExptTcl::setStartCommand );

    defSetter("addBlock", &ExptDriver::addBlock);
    defAttrib("autosaveFile",
              &ExptDriver::getAutosaveFile, &ExptDriver::setAutosaveFile);
    defAttrib("autosavePeriod",
              &ExptDriver::getAutosavePeriod,
              &ExptDriver::setAutosavePeriod);
    defGetter("blocks", &ExptDriver::blocks);
    defAction("clear", &ExptDriver::edClearExpt);
    defGetter("currentBlock", &ExptDriver::currentBlock);
    defGetter("infoLog", &ExptDriver::getInfoLog);
    defAction("reset", &ExptDriver::edResetExpt);
    defAction("stop", &ExptDriver::vxHalt);
    defAction("storeData", &ExptDriver::storeData);
    defAttrib("widget", &ExptDriver::getWidget, &ExptDriver::setWidget);
    defGetter("doWhenComplete", &ExptDriver::getDoWhenComplete);
    defSetter("doWhenComplete", &ExptDriver::setDoWhenComplete);

    Pkg::eval("foreach cmd [info commands ::Exp::*] {"
              "  namespace eval Expt { proc [namespace tail $cmd] {args} \" eval $cmd \\[Exp::currentExp\\] \\$args \" } }\n"
              "namespace eval Expt { namespace export * }"
              );
  }

  virtual ~ExpPkg()
    {
      if (theExptDriver.isValid())
        theExptDriver->edClearExpt();
    }
};

//---------------------------------------------------------------------
//
// Expt_Init --
//
//---------------------------------------------------------------------

extern "C"
int Expt_Init(Tcl_Interp* interp)
{
DOTRACE("Expt_Init");

  Tcl::Pkg* pkg = new ExptTcl::ExpPkg(interp);

  Tcl::defCreator<ExptDriver>(pkg, "Expt");

  return pkg->initStatus();
}

static const char vcid_expttcl_cc[] = "$Header$";
#endif // !EXPTTCL_CC_DEFINED
