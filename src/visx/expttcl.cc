///////////////////////////////////////////////////////////////////////
//
// expttcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  8 03:18:40 1999
// written: Sun Jul 15 17:35:38 2001
// $Id$
//
// This file defines the procedures that provide the Tcl interface to
// the ExptDriver class, and defines several Tcl variables that are
// provided within the Tcl Expt:: namespace.
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_CC_DEFINED
#define EXPTTCL_CC_DEFINED

#include "application.h"
#include "block.h"
#include "exptdriver.h"

#include "gwt/widget.h"

#include "system/system.h"

#include "tcl/genericobjpkg.h"
#include "tcl/objfunctor.h"
#include "tcl/tclevalcmd.h"
#include "tcl/tclitempkg.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"
#include "util/ref.h"
#include "util/strings.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Expt Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace ExptTcl
{
  class PauseCmd;
  class ExpPkg;

  WeakRef<ExptDriver> theExptDriver;

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
    GWT::Widget& widget = expt->getWidget();

    // Create the begin key binding
    widget.bind("<Control-KeyPress-b>", "{ Togl::takeFocus; Expt::begin }");

    // Create the quit key binding
    widget.bind("<Control-KeyPress-q>", "{ Expt::storeData; exit }");

    // Create the save key binding
    widget.bind("<Control-KeyPress-s>", "{ Expt::storeData }");

    // Create the stop key binding
    widget.bind("<Control-KeyPress-c>", "{ Expt::stop }");

    // Create the reset key binding
    widget.bind("<Control-KeyPress-r>", "{ Expt::reset }");

    // Create the pause key binding
    widget.bind("<KeyPress-p>", "{ Expt::pause }");

    // Destroy the experiment start key binding
    widget.bind("<KeyPress-s>", "{}");

    // Force the focus to the Togl widget
    widget.takeFocus();

    expt->edBeginExpt();
  }

  void setStartCommand(Ref<ExptDriver> expt, const char* command)
  {
    // Build the script to be executed when the start key is pressed
    dynamic_string start_command = "{ ";
    start_command += command;
    start_command += " }";

    GWT::Widget& widget = expt->getWidget();

    widget.bind("<KeyPress-s>", start_command.c_str());
    widget.takeFocus();
  }
}

///////////////////////////////////////////////////////////////////////
//
// Expt Tcl definitions
//
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
//
// PauseCmd --
//
// Tell the ExptDriver to halt the experiment, then pop up a pause
// window. When the user dismisses the window, the experiment will
// resume.
//
//--------------------------------------------------------------------

class ExptTcl::PauseCmd : public Tcl::TclCmd {
public:
  PauseCmd(Tcl::TclPkg* pkg, const char* cmd_name) :
    Tcl::TclCmd(pkg->interp(), cmd_name, "expt_id", 2),
    itsPauseMsgCmd(
            "tk_messageBox -default ok -icon info "
            "-title \"Pause\" -type ok "
            "-message \"Experiment paused. Click OK to continue.\";\n",
            Tcl::TclEvalCmd::THROW_EXCEPTION)
  {}

protected:
  virtual void invoke(Tcl::Context& ctx)
  {
    Ref<ExptDriver> ed(ctx.getValFromArg(1, (unsigned int*)0));
    ed->edHaltExpt();

    ed->addLogInfo("Experiment paused.");

    itsPauseMsgCmd.invoke(ctx.interp());

    // Clear the event queue
    while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0)
      { }

    GWT::Widget& widget = ed->getWidget();
    widget.clearscreen();
    widget.swapBuffers();
    widget.clearscreen();
    widget.swapBuffers();

    System::theSystem().sleep(2);

    widget.clearscreen();
    widget.swapBuffers();
    widget.clearscreen();
    widget.swapBuffers();

    // Clear the event queue
    while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0)
      { }

    ed->addLogInfo("Resuming experiment.");

    ed->edResumeExpt();
  }

private:
  Tcl::TclEvalCmd itsPauseMsgCmd;
};


//---------------------------------------------------------------------
//
// ExpPkg definition
//
//---------------------------------------------------------------------

class ExptTcl::ExpPkg : public Tcl::GenericObjPkg<ExptDriver>
{
private:

public:
  ExpPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<ExptDriver>(interp, "Exp", "$Revision$")
  {
    theExptDriver = Ref<ExptDriver>
      (ExptDriver::make(Application::theApp().argc(),
                        Application::theApp().argv(),
                        interp));

    Tcl::TclItemPkg::addIoCommands();

    Tcl::addTracing(this, ExptDriver::tracer);

    Tcl::def( this, &ExptTcl::setCurrentExpt,
              "Exp::currentExp", "expt_id" );
    Tcl::def( this, &ExptTcl::getCurrentExpt,
              "Exp::currentExp", 0 );

    Tcl::def( this, &ExptTcl::begin, "Exp::begin", "expt_id" );
    addCommand( new PauseCmd(this, "Exp::pause") );
    Tcl::def( this, &ExptTcl::setStartCommand,
              "Expt::setStartCommand", "expt_id start_command" );

    declareCSetter("addBlock", &ExptDriver::addBlock);
    declareCAttrib("autosaveFile",
                   &ExptDriver::getAutosaveFile, &ExptDriver::setAutosaveFile);
    declareCAttrib("autosavePeriod",
                   &ExptDriver::getAutosavePeriod,
                   &ExptDriver::setAutosavePeriod);
    declareCAction("clear", &ExptDriver::edClearExpt);
    declareCGetter("currentBlock", &ExptDriver::currentBlock);
    declareCAction("reset", &ExptDriver::edResetExpt);
    declareCAction("stop", &ExptDriver::edHaltExpt);
    declareCAction("storeData", &ExptDriver::storeData);
    declareCAttrib("widget", &ExptDriver::widget, &ExptDriver::setWidget);

    TclPkg::eval("foreach cmd [info commands ::Exp::*] {"
                 "  proc ::Expt::[namespace tail $cmd] {args} \" eval $cmd \\[Exp::currentExp\\] \\$args \" }\n"
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

namespace {
  Tcl_Interp* exptCreateInterp = 0;

  ExptDriver* makeExptDriver()
  {
    if (exptCreateInterp == 0)
      FactoryError::throwForType("ExptDriver");

    return ExptDriver::make(Application::theApp().argc(),
                            Application::theApp().argv(),
                            exptCreateInterp);
  }
}

extern "C"
int Expt_Init(Tcl_Interp* interp)
{
DOTRACE("Expt_Init");

  Tcl::TclPkg* pkg = new ExptTcl::ExpPkg(interp);

  exptCreateInterp = interp;

  Util::ObjFactory::theOne().registerCreatorFunc( makeExptDriver );
  Util::ObjFactory::theOne().registerAlias( "ExptDriver", "Expt" );

  return pkg->initStatus();
}

static const char vcid_expttcl_cc[] = "$Header$";
#endif // !EXPTTCL_CC_DEFINED
