///////////////////////////////////////////////////////////////////////
//
// expttcl.cc
// Rob Peters
// created: Mon Mar  8 03:18:40 1999
// written: Mon Oct 23 12:29:22 2000
// $Id$
//
// This file defines the procedures that provide the Tcl interface to
// the ExptDriver class, and defines several Tcl variables that are
// provided within the Tcl Expt:: namespace.
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_CC_DEFINED
#define EXPTTCL_CC_DEFINED

#include "grshapp.h"
#include "exptdriver.h"

#include "gwt/widget.h"

#include "system/system.h"

#include "tcl/tclcmd.h"
#include "tcl/tclevalcmd.h"
#include "tcl/tclitempkg.h"
#include "tcl/tracertcl.h"

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

namespace ExptTcl {
  class BeginCmd;
  class InitCmd;
  class PauseCmd;
  class SetStartCommandCmd;

  class ExptPkg;
};

///////////////////////////////////////////////////////////////////////
//
// Expt Tcl definitions
//
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
//
// BeginCmd --
//
// Creates the necessary screen bindings for start, pause, and quit,
// then begins the current trial (probably the first trial) of the
// current Expt. Record the time when the experiment
// began. edBeginExpt is called, which displays a trial, and generates
// the timer callbacks associated with a trial.
//
//--------------------------------------------------------------------

class ExptTcl::BeginCmd : public Tcl::TclItemCmd<ExptDriver> {
public:
  BeginCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<ExptDriver>(pkg, cmd_name, NULL, 1, 1)
	 {}
protected:
  virtual void invoke();
};

void ExptTcl::BeginCmd::invoke() {
DOTRACE("ExptTcl::BeginCmd::beginCmd");

  ExptDriver* ed = getItem();
  GWT::Widget* widget = ed->getWidget();

  // Create the begin key binding
  widget->bind("<Control-KeyPress-b>", "{ Togl::takeFocus; Expt::begin }");

  // Create the quit key binding
  widget->bind("<Control-KeyPress-q>", "{ Expt::storeData; exit }");

  // Create the save key binding
  widget->bind("<Control-KeyPress-s>", "{ Expt::storeData }");

  // Create the stop key binding
  widget->bind("<Control-KeyPress-c>", "{ Expt::stop }");

  // Create the reset key binding
  widget->bind("<Control-KeyPress-r>", "{ Expt::reset }");

  // Create the pause key binding
  widget->bind("<KeyPress-p>", "{ Expt::pause }");

  // Destroy the experiment start key binding
  widget->bind("<KeyPress-s>", "{}");

  // Force the focus to the Togl widget
  widget->takeFocus();

  ed->edBeginExpt();
}

//--------------------------------------------------------------------
//
// PauseCmd --
//
// Tell the ExptDriver to halt the experiment, then pop up a pause
// window. When the user dismisses the window, the experiment will
// resume.
//
//--------------------------------------------------------------------

class ExptTcl::PauseCmd : public Tcl::TclItemCmd<ExptDriver> {
public:
  PauseCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<ExptDriver>(pkg, cmd_name, (char*)0, 1, 1),
	 itsPauseMsgCmd(
				"tk_messageBox -default ok -icon info "
				"-title \"Pause\" -type ok "
				"-message \"Experiment paused. Click OK to continue.\";\n",
				Tcl::TclEvalCmd::THROW_EXCEPTION)
  {}

protected:
  virtual void invoke() {
	 ExptDriver* ed = getItem();
	 ed->edHaltExpt();

	 ed->addLogInfo("Experiment paused.");

	 itsPauseMsgCmd.invoke(interp());

	 // Clear the event queue
	 while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0)
		{ }

	 GWT::Widget* widget = ed->getWidget();
	 widget->clearscreen();
	 widget->swapBuffers();
	 widget->clearscreen();
	 widget->swapBuffers();

	 System::theSystem().sleep(2);

	 widget->clearscreen();
	 widget->swapBuffers();
	 widget->clearscreen();
	 widget->swapBuffers();

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
// SetStartCommandCmd --
//
//---------------------------------------------------------------------

class ExptTcl::SetStartCommandCmd : public Tcl::TclItemCmd<ExptDriver> {
public:
  SetStartCommandCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<ExptDriver>(pkg, cmd_name, "start_command", 2, 2) {}
protected:
  virtual void invoke() {
	 // Build the script to be executed when the start key is pressed
	 dynamic_string start_command = "{ ";
	 start_command += getCstringFromArg(1);
	 start_command += " }";

	 ExptDriver* ed = getItem();
	 GWT::Widget* widget = ed->getWidget();

	 widget->bind("<KeyPress-s>", start_command.c_str());
	 widget->takeFocus();
  }
};

//---------------------------------------------------------------------
//
// ExptPkg class defintion
//
//---------------------------------------------------------------------

class ExptTcl::ExptPkg : public Tcl::CTclIoItemPkg<ExptDriver> {
public:
  ExptPkg(Tcl_Interp* interp);

  virtual IO::IoObject& getIoFromId(int) { return itsExptDriver; }

  virtual ExptDriver* getCItemFromId(int) {
	 return &itsExptDriver;
  }

private:
  ExptDriver itsExptDriver;
};

ExptTcl::ExptPkg::ExptPkg(Tcl_Interp* interp) :
  Tcl::CTclIoItemPkg<ExptDriver>(interp, "Expt", "2.7", 0),
  itsExptDriver(Application::theApp().argc(),
					 Application::theApp().argv(),
					 interp)
{
  DOTRACE("ExptPkg::ExptPkg");

  Tcl::addTracing(this, ExptDriver::tracer);

  addCommand( new BeginCmd(this, "Expt::begin") );
  addCommand( new PauseCmd(this, "Expt::pause") );
  addCommand( new SetStartCommandCmd(this, "Expt::setStartCommand") );

  declareCAttrib("autosaveFile",
					  &ExptDriver::getAutosaveFile, &ExptDriver::setAutosaveFile);
  declareCAttrib("autosavePeriod",
					  &ExptDriver::getAutosavePeriod,
					  &ExptDriver::setAutosavePeriod);
  declareCAction("reset", &ExptDriver::edResetExpt);
  declareCAction("stop", &ExptDriver::edHaltExpt);
  declareCAction("storeData", &ExptDriver::storeData);

  // Install the experiment into the application
  Application& app = Application::theApp();
  GrshApp* grshapp = dynamic_cast<GrshApp*>(&app);

  if (grshapp != 0) {
	 grshapp->installExperiment(&itsExptDriver);
  }
}

//---------------------------------------------------------------------
//
// Expt_Init --
//
//---------------------------------------------------------------------

extern "C" int Expt_Init(Tcl_Interp* interp) {
DOTRACE("Expt_Init");

  new ExptTcl::ExptPkg(interp);

  return TCL_OK;
}

static const char vcid_expttcl_cc[] = "$Header$";
#endif // !EXPTTCL_CC_DEFINED
