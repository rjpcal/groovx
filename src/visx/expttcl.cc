///////////////////////////////////////////////////////////////////////
//
// expttcl.cc
// Rob Peters
// created: Mon Mar  8 03:18:40 1999
// written: Wed Jun 23 19:05:07 1999
// $Id$
//
// This file defines the procedures that provide the Tcl interface to
// the Expt class, and defines several Tcl variables that are provided
// within the Tcl Expt:: namespace.
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_CC_DEFINED
#define EXPTTCL_CC_DEFINED

#include "expttcl.h"

#include <iostream.h>
#include <fstream.h>
#include <tcl.h>
#include <string>

#include "tclevalcmd.h"
#include "exptdriver.h"
#include "tclitempkg.h"
#include "tclcmd.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// Expt Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace ExptTcl {
  class BeginCmd;
  class InitCmd;
  class PauseCmd;
  class ReadCmd;
  class WriteCmd;

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
// began. edBeginTrial is called, which displays a trial, and
// generates the timer callbacks associated with a trial.
//
//--------------------------------------------------------------------

class ExptTcl::BeginCmd : public TclItemCmd<ExptDriver> {
public:
  BeginCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ExptDriver>(pkg, cmd_name, NULL, 1, 1),
	 itsInterp(pkg->interp()) {}
protected:
  virtual void invoke();
private:
  Tcl_Interp* itsInterp;
};

void ExptTcl::BeginCmd::invoke() {
DOTRACE("BeginCmd::beginCmd");
  int result = TCL_OK;

  // Create the quit key binding
  static TclEvalCmd bindQuitCmd(
			"bind .togl <KeyPress-q> { Expt::writeAndExit }");

  result = bindQuitCmd.invoke(itsInterp);
  if (result != TCL_OK) throw TclError();

  // Create the pause key binding
  static TclEvalCmd bindPauseCmd("bind .togl <KeyPress-p> { Expt::pause }");

  result = bindPauseCmd.invoke(itsInterp);
  if (result != TCL_OK) throw TclError();

  // Destroy the experiment start key binding
  static TclEvalCmd unbindStartCmd("bind .togl <KeyPress-s> {}");

  result = unbindStartCmd.invoke(itsInterp);
  if (result != TCL_OK) throw TclError();

  // Force the focus to the Togl widget
  static TclEvalCmd focusForceCmd("focus -force .togl");

  result = focusForceCmd.invoke(itsInterp);
  if (result != TCL_OK) throw TclError();

  ExptDriver* ed = getItem();

  ed->init();
  ed->edBeginTrial();

  returnVoid();
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

class ExptTcl::PauseCmd : public TclItemCmd<ExptDriver> {
public:
  PauseCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ExptDriver>(pkg, cmd_name, NULL, 1, 1),
	 itsInterp(pkg->interp()) {}
protected:
  virtual void invoke() {
	 ExptDriver* ed = getItem();
	 ed->edHaltExpt();

	 static TclEvalCmd pauseMsgCmd(
				"tk_messageBox -default ok -icon info "
				"-title \"Pause\" -type ok "
				"-message \"Experiment paused. Click OK to continue.\";\n"
				"after 1000");

	 int result = pauseMsgCmd.invoke(itsInterp);
	 if (result != TCL_OK) throw TclError();

	 ed->edBeginTrial();
  }
private:
  Tcl_Interp* itsInterp;
};

//--------------------------------------------------------------------
//
// ReadCmd --
//
// Restore the current Expt to a previously saved state by reading a
// file. Returns an error if the filename provided is invalid, or if
// the file is corrupt or contains invalid data. The current Expt and
// all of its components (Tlist, ObjList, PosList) are restored to the
// state described in the file.
//
//--------------------------------------------------------------------

class ExptTcl::ReadCmd : public TclItemCmd<ExptDriver> {
public:
  ReadCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ExptDriver>(pkg, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
	 ExptDriver* ed = getItem();
	 const char* filename = getCstringFromArg(1);

	 try {
		ifstream ifs(filename);
		if (ifs.fail()) throw IoFilenameError(filename);
		ed->deserialize(ifs, IO::BASES|IO::TYPENAME);
	 }
	 catch (IoError& err) {
		throw TclError(err.info());
	 }
	 returnVoid();
  }
};

//---------------------------------------------------------------------
//
// WriteCmd --
//
//---------------------------------------------------------------------

class ExptTcl::WriteCmd : public TclItemCmd<ExptDriver> {
public:
  WriteCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ExptDriver>(pkg, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
  DOTRACE("WriteCmd::invoke");
    const char* filename = getCstringFromArg(1);
	 ExptDriver* ed = getItem();
	 ed->write(filename);
  }
};

//---------------------------------------------------------------------
//
// ExptPkg class defintion
//
//---------------------------------------------------------------------

class ExptTcl::ExptPkg : public CTclIoItemPkg<ExptDriver> {
public:
  ExptPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<ExptDriver>(interp, "Expt", "2.6", 0)
  {
  DOTRACE("ExptPkg::ExptPkg");

	 ExptDriver::theExptDriver().setInterp(interp);
	 
    declareAttrib("autosaveFile", new CAttrib<ExptDriver, const string&>(
                                          &ExptDriver::getAutosaveFile,
														&ExptDriver::setAutosaveFile));
	 addCommand( new BeginCmd(this, "Expt::begin") );
	 addCommand( new PauseCmd(this, "Expt::pause") );
	 addCommand( new ReadCmd(this, "Expt::read") );
	 declareAction("stop", new CAction<ExptDriver>(&ExptDriver::edHaltExpt));
	 addCommand( new WriteCmd(this, "Expt::write") );
	 declareAction("writeAndExit",
						new CAction<ExptDriver>(&ExptDriver::writeAndExit));
  }

  virtual IO& getIoFromId(int) { return ExptDriver::theExptDriver(); }
  virtual int getBufSize() { return 4096; }

  virtual ExptDriver* getCItemFromId(int) {
	 return &ExptDriver::theExptDriver();
  }
};

//---------------------------------------------------------------------
//
// Expt_Init --
//
//---------------------------------------------------------------------

int Expt_Init(Tcl_Interp* interp) {
DOTRACE("Expt_Init");

  new ExptTcl::ExptPkg(interp);

  return TCL_OK;
}

static const char vcid_expttcl_cc[] = "$Header$";
#endif // !EXPTTCL_CC_DEFINED
