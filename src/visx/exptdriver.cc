///////////////////////////////////////////////////////////////////////
//
// exptdriver.cc
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Thu Jun 17 10:52:41 1999
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
#include "tclevalcmd.h"
#include "timinghandler.h"
#include "tlisttcl.h"
#include "trial.h"
#include "blocklist.h"
#include "rhlist.h"
#include "thlist.h"

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

    static TclEvalCmd dateStringCmd("clock format [clock seconds]");

	 int result = dateStringCmd.invoke(interp);
	 if (result != TCL_OK) return string();
	 
	 return string(Tcl_GetStringResult(interp));
  }

};

///////////////////////////////////////////////////////////////////////
//
// ExptDriver creator method definitions
//
///////////////////////////////////////////////////////////////////////

ExptDriver::ExptDriver() :
  itsInterp(0),
  itsAutosaveFile("__autosave_file"),
  itsBlockId(0),
  itsRhId(0),
  itsThId(0)
{
DOTRACE("ExptDriver::ExptDriver");
  Tcl_Preserve(itsInterp);
}

ExptDriver ExptDriver::theInstance;

ExptDriver& ExptDriver::theExptDriver() {
DOTRACE("ExptDriver::theExptDriver");
  return theInstance;
}

ExptDriver::~ExptDriver() {
DOTRACE("ExptDriver::~ExptDriver");
//   Tcl_Release(itsInterp);
}

void ExptDriver::init(int repeat, int seed, const string& date,
							 const string& host, const string& subject) {
DOTRACE("ExptDriver::init");
  itsBeginDate = date;
  itsHostname = host;
  itsSubject = subject;
  expt().init(repeat, seed);

  itsAutosaveFile = "__autosave_file";
  DebugEvalNL(itsAutosaveFile);
}

void ExptDriver::serialize(ostream &os, IOFlag flag) const {
DOTRACE("ExptDriver::serialize");
  expt().serialize(os, flag);

  // itsBeginDate
  os << itsBeginDate << '\n';
  // itsHostname
  os << itsHostname << '\n';
  // itsSubject
  os << itsSubject << '\n';

  os << itsEndDate << '\n';

  os << getAutosaveFile() << '\n';

  responseHandler().serialize(os, flag);

  os << getVerbose() << ' ';

  timingHandler().serialize(os, flag);

  if (os.fail()) throw OutputError(ioTag);
}

void ExptDriver::deserialize(istream &is, IOFlag flag) {
DOTRACE("ExptDriver::deserialize");
  expt().deserialize(is, flag);

  // itsBeginDate
  if (is.peek() != EOF) { getline(is, itsBeginDate, '\n'); }
  // itsHostname
  if (is.peek() != EOF) { getline(is, itsHostname, '\n'); }
  // itsSubject
  if (is.peek() != EOF) { getline(is, itsSubject, '\n'); }

  if (is.peek() != EOF) { getline(is, itsEndDate, '\n'); }

  if (is.peek() != EOF) { getline(is, itsAutosaveFile, '\n'); }

  responseHandler().deserialize(is, flag);

  if (is.peek() != EOF) {
	 int aVerbose;
	 is >> aVerbose;
	 setVerbose(bool(aVerbose));
  }

  timingHandler().deserialize(is, flag);

  if (is.fail()) throw InputError(ioTag);
}

int ExptDriver::charCount() const {
DOTRACE("ExptDriver::charCount");
  return (0
			 + itsBeginDate.size() + 1
			 + itsHostname.size() + 1
			 + itsSubject.size() + 1
			 + 5); // fudge factor
}


///////////////////////////////////////////////////////////////////////
//
// ExptDriver accessor + manipulator method definitions
//
///////////////////////////////////////////////////////////////////////

Tcl_Interp* ExptDriver::getInterp() {
DOTRACE("ExptDriver::getInterp");
  return itsInterp;
}


void ExptDriver::setInterp(Tcl_Interp* interp) {
DOTRACE("ExptDriver::setInterp");
  // itsInterp can only be set once
  if (itsInterp == NULL) {
	 itsInterp = interp;
	 Tcl_Preserve(itsInterp);
  }
}

Expt& ExptDriver::expt() {
DOTRACE("ExptDriver::expt");  
#ifdef LOCAL_ASSERT
  Expt* expt = BlockList::theBlockList().getPtr(itsBlockId);
  DebugEvalNL((void *) expt);
  Assert(expt != 0);
  return *expt;
#else
  return *(BlockList::theBlockList().getPtr(itsBlockId));
#endif
}

ResponseHandler& ExptDriver::responseHandler() {
DOTRACE("ExptDriver::responseHandler");
#ifdef LOCAL_ASSERT
  DebugEval(itsRhId);
  ResponseHandler* rh = RhList::theRhList().getPtr(itsRhId);
  DebugEvalNL((void *) rh);
  Assert(rh != 0);
  return *rh;
#else
  return *(RhList::theRhList().getPtr(itsRhId));
#endif
}

TimingHandler& ExptDriver::timingHandler() {
DOTRACE("ExptDriver::timingHandler");
#ifdef LOCAL_ASSERT
  TimingHandler* th = ThList::theThList().getPtr(itsThId);
  DebugEvalNL((void *) th);
  Assert(th != 0);
  return *th;
#else
  return *(ThList::theThList().getPtr(itsThId));
#endif
}

const Expt& ExptDriver::expt() const {
DOTRACE("ExptDriver::expt const");
  return const_cast<ExptDriver *>(this)->expt();
}

const ResponseHandler& ExptDriver::responseHandler() const {
DOTRACE("ExptDriver::responseHandler const");
  return const_cast<ExptDriver *>(this)->responseHandler();
}

const TimingHandler& ExptDriver::timingHandler() const {
DOTRACE("ExptDriver::timingHandler const");
  return const_cast<ExptDriver *>(this)->timingHandler();
}

void ExptDriver::setResponseHandler(int rhid) {
DOTRACE("ExptDriver::setResponseHandler");
  DebugEval(itsRhId);
  DebugEvalNL(rhid);
  itsRhId = rhid;
}

void ExptDriver::setTimingHandler(int thid) {
DOTRACE("ExptDriver::setTimingHandler");
  itsThId = thid;
}

const string& ExptDriver::getAutosaveFile() const { return itsAutosaveFile; }
bool ExptDriver::getVerbose() const { return expt().getVerbose(); }

void ExptDriver::setAutosaveFile(const string& str) { itsAutosaveFile = str; }
void ExptDriver::setVerbose(bool val) { expt().setVerbose(val); }

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
  int period = timingHandler().getAutosavePeriod();
  return ( (period > 0) && ((expt().numCompleted() % period) == 0) );
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
  if (expt().isComplete()) return;

  Trial& trial = expt().getCurTrial();

  itsThId = trial.getTimingHandler();
  DebugEval(itsThId);

  itsRhId = trial.getResponseHandler();
  DebugEval(itsRhId);

  responseHandler().rhBeginTrial();
  timingHandler().thBeginTrial();
  expt().beginTrial();
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
  responseHandler().rhAbortTrial();
  expt().abortTrial();
  timingHandler().thAbortTrial();
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
  timingHandler().thEndTrial();
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
  expt().drawTrial();
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
  expt().undrawTrial();
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
  Assert(itsInterp != 0);

  // If resp is -1, then the keypress did not match any of the regexps
  // in theRegexps, so the response was invalid, and we abort the
  // trial.
  if (response == -1) {
    edAbortTrial();
    return;
  }

  expt().processResponse(response);

  if ( needAutosave() ) {
	 DebugEvalNL(getAutosaveFile().c_str());

	 int result = write(getAutosaveFile().c_str());
	 if (result != TCL_OK) Tcl_BackgroundError(itsInterp);
  }

  if (expt().isComplete()) {
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
  timingHandler().thHaltExpt();

  responseHandler().rhHaltExpt();

  expt().undrawTrial();
  expt().abortTrial();
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
  timingHandler().thResponseSeen();

  expt().undrawTrial();
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
  Assert(itsInterp != 0);

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
  Assert(itsInterp != 0);

  // Get the current date/time and record it as the ending date for
  // thec current Expt.
  itsEndDate = getDateStringProc(itsInterp);
  if (itsEndDate.size() == 0) return TCL_ERROR;

  // Parse the ending date for the current Expt and format into a
  // unique filename extension
  static TclEvalCmd uniqueFilenameCmd(
        "clock format [clock scan [Expt::endDate]] -format %H%M%d%b%Y");

  int result = uniqueFilenameCmd.invoke(itsInterp);
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
