///////////////////////////////////////////////////////////////////////
//
// exptdriver.cc
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Thu Jun 24 15:23:55 1999
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

#include "tclerror.h"
#include "expt.h"
#include "responsehandler.h"
#include "tclevalcmd.h"
#include "timinghdlr.h"
#include "tlisttcl.h"
#include "trial.h"
#include "blocklist.h"
#include "rhlist.h"
#include "thlist.h"
#include "tlist.h"

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

void ExptDriver::init() {
DOTRACE("ExptDriver::init");

  int result;

  // Get the current time/date
  static TclEvalCmd dateStringCmd("clock format [clock seconds]");

  result = dateStringCmd.invoke(itsInterp);
  if (result != TCL_OK) throw TclError("couldn't get date");

  itsBeginDate = Tcl_GetStringResult(itsInterp);

  // Get the hostname
  itsHostname = Tcl_GetVar2(itsInterp, "env", "HOST",
									 TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG);;

  // Get the subject's initials as the tail of the current directory
  static TclEvalCmd subjectKeyCmd("file tail [pwd]");

  result = subjectKeyCmd.invoke(itsInterp);
  if (result != TCL_OK) throw TclError("couldn't get subject initials");

  // Get the result, and remove an optional leading 'human_', if present
  const char* key = Tcl_GetStringResult(itsInterp);
  if ( strncmp(key, "human_", 6) == 0 ) {
    key += 6;
  }

  itsSubject = key;

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

  timingHdlr().serialize(os, flag);

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

  timingHdlr().deserialize(is, flag);

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

TimingHdlr& ExptDriver::timingHdlr() {
DOTRACE("ExptDriver::timingHdlr");
#ifdef LOCAL_ASSERT
  TimingHdlr* th = ThList::theThList().getPtr(itsThId);
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

const TimingHdlr& ExptDriver::timingHdlr() const {
DOTRACE("ExptDriver::timingHdlr const");
  return const_cast<ExptDriver *>(this)->timingHdlr();
}

void ExptDriver::setResponseHandler(int rhid) {
DOTRACE("ExptDriver::setResponseHandler");
  DebugEval(itsRhId);
  DebugEvalNL(rhid);
  itsRhId = rhid;
}

void ExptDriver::setTimingHdlr(int thid) {
DOTRACE("ExptDriver::setTimingHdlr");
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
  int period = timingHdlr().getAutosavePeriod();
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

  itsThId = trial.getTimingHdlr();
  DebugEval(itsThId);

  itsRhId = trial.getResponseHandler();
  DebugEval(itsRhId);

  expt().beginTrial();
  timingHdlr().thBeginTrial();
  responseHandler().rhBeginTrial();
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
  timingHdlr().thAbortTrial();
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

//---------------------------------------------------------------------
//
// ExptDriver::edSwapBuffers --
//
// Swap the buffers in the Togl widget
//
//---------------------------------------------------------------------

void ExptDriver::edSwapBuffers() {
DOTRACE("ExptDriver::edSwapBuffers");
  static TclEvalCmd swapCmd(".togl swapbuffers");
  int result = swapCmd.invoke(itsInterp);
  if (result != TCL_OK) Tcl_BackgroundError(itsInterp);
}

void ExptDriver::edClearBuffer() const {
DOTRACE("ExptDriver::edClearBuffer");
  Tlist::theTlist().clearBuffer();
}

void ExptDriver::edRenderBack() const {
DOTRACE("ExptDriver::edRenderBack");
  Tlist::theTlist().renderBack();
}

void ExptDriver::edRenderFront() const {
DOTRACE("ExptDriver::edRenderFront");
  Tlist::theTlist().renderFront();
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

	 try {
		write(getAutosaveFile().c_str());
	 }
	 catch (TclError&) {
		Tcl_BackgroundError(itsInterp);
	 }
  }

  if (expt().isComplete()) {
    cout << "expt complete" << endl;
    ExptDriver::writeAndExit();
	 return; // We'll never get here since writeAndExit calls exit()
  }    
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
  timingHdlr().thHaltExpt();

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
  timingHdlr().thResponseSeen();

  // This is commented out because undrawing the Trial on
  // response-seen should be the responsibility of the TimingHandler.
//   expt().undrawTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::read --
//
//--------------------------------------------------------------------

void ExptDriver::read(const char* filename) {
DOTRACE("ExptDriver::read");
  try {
	 ifstream ifs(filename);
	 if (ifs.fail()) throw IoFilenameError(filename);
	 deserialize(ifs, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
	 DebugEvalNL(err.msg());
	 throw TclError(err.msg());
  }
}

//--------------------------------------------------------------------
//
// ExptDriver::write --
//
//--------------------------------------------------------------------

void ExptDriver::write(const char* filename) const {
DOTRACE("ExptDriver::write");
  try {
    ofstream ofs(filename);
    if (ofs.fail()) throw IoFilenameError(filename);
	 serialize(ofs, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
	 DebugEvalNL(err.msg());
	 throw TclError(err.msg());
  }
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

void ExptDriver::writeAndExit() {
DOTRACE("ExptDriver::writeAndExit");
  Assert(itsInterp != 0);

  // Get the current date/time and record it as the ending date for
  // thec current Expt.
  itsEndDate = getDateStringProc(itsInterp);
  if (itsEndDate.size() == 0) throw TclError("couldn't get date");

  // Format the current time into a unique filename extension
  static TclEvalCmd uniqueFilenameCmd(
        "clock format [clock seconds] -format %H%M%d%b%Y");

  int result = uniqueFilenameCmd.invoke(itsInterp);
  if (result != TCL_OK) throw TclError();
  string unique_filename = Tcl_GetStringResult(itsInterp);

  // Write the main experiment file
  string expt_filename = string("expt") + unique_filename;

  write(expt_filename.c_str());

  cout << "wrote file " << expt_filename << endl;

  // Write the responses file
  string resp_filename = string("resp") + unique_filename;

  result = TlistTcl::write_responsesProc(itsInterp, NULL, 
													  resp_filename.c_str());
  if (result != TCL_OK) throw TclError();

  cout << "wrote file " << resp_filename << endl;

  // Gracefully exit the application (or not)
//   Tcl_Exit(0);
}

static const char vcid_exptdriver_cc[] = "$Header$";
#endif // !EXPTDRIVER_CC_DEFINED
