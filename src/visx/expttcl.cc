///////////////////////////////////////////////////////////////////////
//
// expttcl.cc
// Rob Peters
// created: Mon Mar  8 03:18:40 1999
// written: Thu Nov 11 19:25:21 1999
// $Id$
//
// This file defines the procedures that provide the Tcl interface to
// the ExptDriver class, and defines several Tcl variables that are
// provided within the Tcl Expt:: namespace.
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_CC_DEFINED
#define EXPTTCL_CC_DEFINED

#include <tcl.h>
#include <string>
#include <fstream.h>

#include "asciistreamreader.h"
#include "asciistreamwriter.h"
#include "tclevalcmd.h"
#include "exptdriver.h"
#include "tclitempkg.h"
#include "tclcmd.h"
#include "objtogl.h"
#include "toglconfig.h"

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
  class SetStartCommandCmd;
  class WriteCmd;

  class LoadCmd;
  class SaveCmd;

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

  ToglConfig* config = ObjTogl::theToglConfig();

  // Create the quit key binding
  config->bind("<Control-KeyPress-q>", "{ Expt::writeAndExit yes}");

  // Create the save key binding
  config->bind("<Control-KeyPress-s>", "{ Expt::writeAndExit no}");

  // Create the pause key binding
  config->bind("<KeyPress-p>", "{ Expt::pause }");

  // Destroy the experiment start key binding
  config->bind("<KeyPress-s>", "{}");

  // Force the focus to the Togl widget
  config->takeFocus();

  ExptDriver* ed = getItem();

  ed->edBeginExpt();

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
				"after 1000",
				TclEvalCmd::THROW_EXCEPTION);

	 pauseMsgCmd.invoke(itsInterp);

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

	 ed->read(filename);
	 returnVoid();
  }
};

//---------------------------------------------------------------------
//
// SetStartCommandCmd --
//
//---------------------------------------------------------------------

class ExptTcl::SetStartCommandCmd : public TclItemCmd<ExptDriver> {
public:
  SetStartCommandCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ExptDriver>(pkg, cmd_name, "start_command", 2, 2) {}
protected:
  virtual void invoke() {
	 // Build the script to be executed when the start key is pressed
	 string start_command = "{ ";
	 start_command += getCstringFromArg(1);
	 start_command += " }";

	 ToglConfig* config = ObjTogl::theToglConfig();

	 config->bind("<KeyPress-s>", start_command.c_str());
	 config->takeFocus();
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
	 ExptDriver* ed = getItem();
    const char* filename = getCstringFromArg(1);

	 ed->write(filename);
	 returnVoid();
  }
};

//---------------------------------------------------------------------
//
// LoadCmd --
//
//---------------------------------------------------------------------

class ExptTcl::LoadCmd : public TclItemCmd<ExptDriver> {
public:
  LoadCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ExptDriver>(pkg, cmd_name, "input_filename", 2, 2) {}
protected:
  virtual void invoke() {
	 ExptDriver* ed = getItem();
	 const char* filename = getCstringFromArg(1);

	 ifstream ifs(filename);
	 if ( ifs.fail() ) { throw TclError("non-existent or unreadable file"); }

	 AsciiStreamReader reader(ifs);
	 reader.readRoot(ed);
  }
};

//---------------------------------------------------------------------
//
// LoadCmd --
//
//---------------------------------------------------------------------

class ExptTcl::SaveCmd : public TclItemCmd<ExptDriver> {
public:
  SaveCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ExptDriver>(pkg, cmd_name, "output_filename", 2, 2) {}
protected:
  virtual void invoke() {
	 ExptDriver* ed = getItem();
	 const char* filename = getCstringFromArg(1);

	 ofstream ofs(filename);
	 if ( ofs.fail() ) { throw TclError("couldn't open file for writing"); }

	 AsciiStreamWriter writer(ofs);
	 writer.writeRoot(ed);
	 
	 returnVoid();
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
	 CTclIoItemPkg<ExptDriver>(interp, "Expt", "2.7", 0)
  {
  DOTRACE("ExptPkg::ExptPkg");

	 ExptDriver::theExptDriver().setInterp(interp);
	 
	 addCommand( new BeginCmd(this, "Expt::begin") );
	 addCommand( new PauseCmd(this, "Expt::pause") );
	 addCommand( new ReadCmd(this, "Expt::read") );
	 addCommand( new SetStartCommandCmd(this, "Expt::setStartCommand") );
	 addCommand( new WriteCmd(this, "Expt::write") );

	 addCommand( new LoadCmd(this, "Expt::load") );
	 addCommand( new SaveCmd(this, "Expt::save") );

    declareCAttrib("autosaveFile",
						 &ExptDriver::getAutosaveFile, &ExptDriver::setAutosaveFile);
	 declareCAction("reset", &ExptDriver::edResetExpt);
	 declareCAction("stop", &ExptDriver::edHaltExpt);
	 declareCSetter("writeAndExit", &ExptDriver::writeAndExit);
  }

  virtual IO& getIoFromId(int) { return ExptDriver::theExptDriver(); }

  virtual ExptDriver* getCItemFromId(int) {
	 return &ExptDriver::theExptDriver();
  }
};

//---------------------------------------------------------------------
//
// Expt_Init --
//
//---------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Expt_Init;

int Expt_Init(Tcl_Interp* interp) {
DOTRACE("Expt_Init");

  new ExptTcl::ExptPkg(interp);

  return TCL_OK;
}

static const char vcid_expttcl_cc[] = "$Header$";
#endif // !EXPTTCL_CC_DEFINED
