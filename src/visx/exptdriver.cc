///////////////////////////////////////////////////////////////////////
//
// exptdriver.cc
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Wed Jun  9 19:35:04 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_CC_DEFINED
#define EXPTDRIVER_CC_DEFINED

#include "exptdriver.h"

#include <tcl.h>
#include <iostream.h>
#include <fstream.h>
#include <string>

#include "errmsg.h"
#include "expt.h"
#include "responsehandler.h"
#include "tclobjlock.h"
#include "timinghandler.h"
#include "tlisttcl.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {

  const string ioTag = "Expt";

  string getDateStringProc(Tcl_Interp* interp) {
  DOTRACE("getDateStringProc");
    static Tcl_Obj* dateStringCmdObj =
		Tcl_NewStringObj("clock format [clock seconds]", -1);
	 static TclObjLock lock_(dateStringCmdObj);
	 
	 int result = Tcl_EvalObjEx(interp, dateStringCmdObj, TCL_EVAL_GLOBAL);
	 if (result != TCL_OK) return string();
	 
	 return string(Tcl_GetStringResult(interp));
  }

};

///////////////////////////////////////////////////////////////////////
//
// ExptDriver creator method definitions
//
///////////////////////////////////////////////////////////////////////

ExptDriver::ExptDriver(Tcl_Interp* interp) :
  itsExpt( *(new Expt(0)) ),
  itsInterp(interp),
  itsAutosaveFile("__autosave_file"),
  itsResponseHandler( *(new ResponseHandler(*this, interp)) ),
  itsTimingHandler( *(new TimingHandler(*this)) )
{
DOTRACE("ExptDriver::ExptDriver");
  Tcl_Preserve(itsInterp);
}

ExptDriver::~ExptDriver() {
DOTRACE("ExptDriver::~ExptDriver");
  delete &itsExpt;
  delete &itsResponseHandler;
  delete &itsTimingHandler;
  Tcl_Release(itsInterp);
}

void ExptDriver::init(int repeat, int seed, const string& date,
							 const string& host, const string& subject) {
DOTRACE("ExptDriver::init");
  itsExpt.init(repeat, seed, date, host, subject);

  itsAutosaveFile = "__autosave_file";
  DebugEvalNL(itsAutosaveFile);
}

void ExptDriver::serialize(ostream &os, IOFlag flag) const {
DOTRACE("ExptDriver::serialize");
  itsExpt.serialize(os, flag);

  os << getEndDate() << '\n';

  os << getAutosaveFile() << '\n';

  itsResponseHandler.serialize(os, flag);

  os << getVerbose() << ' ';

  itsTimingHandler.serialize(os, flag);

  if (os.fail()) throw OutputError(ioTag);
}

void ExptDriver::deserialize(istream &is, IOFlag flag) {
DOTRACE("ExptDriver::deserialize");
  itsExpt.deserialize(is, flag);

  if (is.peek() != EOF) { 
	 string aEndDate;
	 getline(is, aEndDate, '\n');
	 setEndDate(aEndDate);
  }

  if (is.peek() != EOF) {
	 string aAutosaveFile;
	 getline(is, aAutosaveFile, '\n');
	 setAutosaveFile(aAutosaveFile);
  }

  itsResponseHandler.deserialize(is, flag);

  if (is.peek() != EOF) {
	 int aVerbose;
	 is >> aVerbose;
	 setVerbose(bool(aVerbose));
  }

  itsTimingHandler.deserialize(is, flag);

  if (is.fail()) throw InputError(ioTag);
}

int ExptDriver::charCount() const {
DOTRACE("ExptDriver::charCount");
  return 0;
}


///////////////////////////////////////////////////////////////////////
//
// ExptDriver accessor + manipulator method definitions
//
///////////////////////////////////////////////////////////////////////

const string& ExptDriver::getEndDate() const { return itsEndDate; }
const string& ExptDriver::getAutosaveFile() const { return itsAutosaveFile; }
bool ExptDriver::getVerbose() const { return itsExpt.getVerbose(); }

int ExptDriver::getAbortWait() const { return itsTimingHandler.getAbortWait(); }
int ExptDriver::getAutosavePeriod() const { 
  return itsTimingHandler.getAutosavePeriod(); 
}
int ExptDriver::getInterTrialInterval() const { 
  return itsTimingHandler.getInterTrialInterval(); 
}
int ExptDriver::getStimDur() const { return itsTimingHandler.getStimDur(); }
int ExptDriver::getTimeout() const { return itsTimingHandler.getTimeout(); }

void ExptDriver::setEndDate(const string& str) { itsEndDate = str; }
void ExptDriver::setAutosaveFile(const string& str) { itsAutosaveFile = str; }
void ExptDriver::setVerbose(bool val) { itsExpt.setVerbose(val); }

void ExptDriver::setAbortWait(int val) { itsTimingHandler.setAbortWait(val); }
void ExptDriver::setAutosavePeriod(int val) { itsTimingHandler.setAutosavePeriod(val); }
void ExptDriver::setInterTrialInterval(int val) 
{ itsTimingHandler.setInterTrialInterval(val); }
void ExptDriver::setStimDur(int val) { itsTimingHandler.setStimDur(val); }
void ExptDriver::setTimeout(int val) { itsTimingHandler.setTimeout(val); }
  
const string& ExptDriver::getKeyRespPairs() const {
DOTRACE("ExptDriver::getKeyRespPairs");
  return itsResponseHandler.getKeyRespPairs();
}

bool ExptDriver::getUseFeedback() const {
DOTRACE("ExptDriver::getUseFeedback");
  return itsResponseHandler.getUseFeedback();
}

//--------------------------------------------------------------------
//
// ExptDriver::needAutosave --
//
// Determine whether an autosave is necessary now. An autosave is
// requested only if itsAutosavePeriod is positive, and the number of
// completed trials is evenly divisible by the autosave period.
//
//--------------------------------------------------------------------

bool ExptDriver::needAutosave() const {
DOTRACE("ExptDriver::needAutosave");
  return ( (getAutosavePeriod() > 0) &&
			  ((itsExpt.numCompleted() % getAutosavePeriod()) == 0) );
}

void ExptDriver::setKeyRespPairs(const string& s) {
DOTRACE("ExptDriver::setKeyRespPairs");
  itsResponseHandler.setKeyRespPairs(s);
}

void ExptDriver::setUseFeedback(bool val) {
DOTRACE("ExptDriver::setUseFeedback"); 
  itsResponseHandler.setUseFeedback(val);
}

///////////////////////////////////////////////////////////////////////
//
// ExptDriver action method definitions
//
///////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------------
//
// ExptDriver::edBeginTrial --
//
// Tell participants to begin the current trial.
//
//--------------------------------------------------------------------

void ExptDriver::edBeginTrial() {
DOTRACE("ExptDriver::edBeginTrial");
  itsResponseHandler.rhBeginTrial();
  itsTimingHandler.thBeginTrial();
  itsExpt.beginTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::edAbortTrial --
//
// Tell particpants to abort this trial.
//
//--------------------------------------------------------------------

void ExptDriver::edAbortTrial() {
DOTRACE("ExptDriver::edAbortTrial");
  itsResponseHandler.rhAbortTrial();
  itsExpt.abortTrial();
  itsTimingHandler.thAbortTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::edEndTrial --
//
// Tell participants to finish up this trial and prepare for the next
// one.
//
//--------------------------------------------------------------------

void ExptDriver::edEndTrial() {
DOTRACE("ExptDriver::edEndTrial");
  itsTimingHandler.thEndTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::draw --
//
// Draw the current Trial by delegation to itsExpt.
//
//--------------------------------------------------------------------

void ExptDriver::draw() {
DOTRACE("ExptDriver::draw");
  itsExpt.drawTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::undraw --
//
// Undraw the current Trial by delegation to itsExpt.
//
//--------------------------------------------------------------------

void ExptDriver::undraw() {
DOTRACE("ExptDriver::undraw");
  itsExpt.undrawTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::edProcessResponse --
//
// Check if the response was valid: if not, then abort the trial; if
// so, then record the response. If the experiment has been completed,
// arrange for files to be written and the program exited; otherwise,
// schedule the next trial.
//
//--------------------------------------------------------------------

void ExptDriver::edProcessResponse(int response) {
DOTRACE("ExptDriver::edProcessResponse");
  // If resp is -1, then the keypress did not match any of the regexps
  // in theRegexps, so the response was invalid, and we abort the
  // trial.
  if (response == -1) {
    edAbortTrial();
    return;
  }

  itsExpt.processResponse(response);

  if ( needAutosave() ) {
	 DebugEvalNL(getAutosaveFile().c_str());

	 int result = write(getAutosaveFile().c_str());
	 if (result != TCL_OK) Tcl_BackgroundError(itsInterp);
  }

  if (itsExpt.isComplete()) {
    cout << "expt complete" << endl;
    ExptDriver::writeAndExit();
	 return; // We'll never get here since writeAndExit calls exit()
  }    
  
  edEndTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::edHaltExpt --
//
// Helper procedure that cancels outstanding timers, unbinds KeyPress
// events from the screen window, clears the screen, and aborts the
// current trial.
//
//--------------------------------------------------------------------

void ExptDriver::edHaltExpt() {
DOTRACE("ExptDriver::edHaltExpt");
  itsTimingHandler.thHaltExpt();

  itsResponseHandler.rhHaltExpt();

  itsExpt.undrawTrial();
  itsExpt.abortTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::edResponseSeen --
//
// This procedure is used to tell the ExptDriver that a response has
// been seen and that it should take appropriate action. This consists
// of cancelling any active timer callbacks, and undrawing the current
// trial from the screen.
//
//--------------------------------------------------------------------

void ExptDriver::edResponseSeen() {
DOTRACE("ExptDriver::edResponseSeen");
  itsTimingHandler.thResponseSeen();

  itsExpt.undrawTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::write --
//
// Write the experiment to a given filename and catch any resulting
// IoError's, turning them into TCL_ERROR.
//
// Results:
// A usual Tcl result.
//
//--------------------------------------------------------------------

int ExptDriver::write(const char* filename) {
DOTRACE("ExptDriver::write");
  try {
    ofstream ofs(filename);
    if (ofs.fail()) throw IoFilenameError(filename);
	 serialize(ofs, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
	 DebugEvalNL(err.info().c_str());

	 err_message(itsInterp, "ExptDriver::write", err.info().c_str());
    return TCL_ERROR;
  }
  return TCL_OK;
}

//--------------------------------------------------------------------
//
// ExptDriver::writeAndExit --
//
// The experiment and a summary of the responses to it are written to
// files with unique filenames, and then the program is gracefully
// exited. This procedure will never return.
//
// Side effects:
// The program is exited--this procedure never returns.
//
//--------------------------------------------------------------------

int ExptDriver::writeAndExit() {
DOTRACE("ExptDriver::writeAndExit");
  // Get the current date/time and record it as the ending date for
  // thec current Expt.
  string date = getDateStringProc(itsInterp);
  if (!date.size()) return TCL_ERROR;
  setEndDate(date);

  // Parse the ending date for the current Expt and format into a
  // unique filename extension
  static Tcl_Obj* uniqueFilenameCmdObj = 
    Tcl_NewStringObj("clock format [clock scan [Expt::endDate]] "
                     "-format %H%M%d%b%Y", -1);
  static TclObjLock lock_(uniqueFilenameCmdObj);

  int result = Tcl_EvalObjEx(itsInterp, uniqueFilenameCmdObj, TCL_EVAL_GLOBAL);
  if (result != TCL_OK) return result;
  string unique_filename = Tcl_GetStringResult(itsInterp);

  // Write the main experiment file
  string expt_filename = string("expt") + unique_filename;

  result = write(expt_filename.c_str());
  if (result != TCL_OK) return result;

  cout << "wrote file " << expt_filename << endl;

  // Write the responses file
  string resp_filename = string("resp") + unique_filename;

  result = TlistTcl::write_responsesProc(itsInterp, NULL, 
													  resp_filename.c_str());
  if (result != TCL_OK) return result;

  cout << "wrote file " << resp_filename << endl;

  // Gracefully exit the application
  Tcl_Exit(0);

  return result;
}

static const char vcid_exptdriver_cc[] = "$Header$";
#endif // !EXPTDRIVER_CC_DEFINED
