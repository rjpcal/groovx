///////////////////////////////////////////////////////////////////////
//
// exptdriver.cc
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Wed Nov  3 13:45:03 1999
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
#include <vector>

#include "tclerror.h"
#include "block.h"
#include "responsehandler.h"
#include "tclevalcmd.h"
#include "timinghdlr.h"
#include "tlisttcl.h"
#include "trial.h"
#include "objlist.h"
#include "objtogl.h"
#include "reader.h"
#include "toglconfig.h"
#include "poslist.h"
#include "blocklist.h"
#include "rhlist.h"
#include "thlist.h"
#include "tlist.h"
#include "timeutils.h"
#include "writer.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

#define TIME_TRACE

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {

  const string ioTag = "ExptDriver";

#ifdef TIME_TRACE
  inline void TimeTraceNL(const char* loc, int msec) {
	 cerr << "in " << loc << ", elapsed time == " << msec << endl;
  }
#else
  inline void TimeTraceNL(const char*, int) {}
#endif
};

///////////////////////////////////////////////////////////////////////
//
// ExptDriver::ExptHelper utility class
//
///////////////////////////////////////////////////////////////////////

class ExptDriver::ExptImpl {
public:
  ExptImpl(ExptDriver* owner);
  ~ExptImpl();

  void serialize(ostream& os, IO::IOFlag flag) const;
  void deserialize(istream& is, IO::IOFlag flag);

  bool doesDoUponCompletionExist() const;

  void raiseBackgroundError(const char* msg) const;
  void raiseBackgroundError(const string& msg) const;

  void doAutosave();

  //////////////////
  // data members //
  //////////////////

private:
  ExptDriver* self;

public:
  Tcl_Interp* itsInterp;

private:
  mutable string itsDoUponCompletionBody;
};

///////////////////////////////////////////////////////////////////////
//
// ExptDriver::ExptHelper member definitions
//
///////////////////////////////////////////////////////////////////////

ExptDriver::ExptImpl::ExptImpl(ExptDriver* owner) :
  itsInterp(0),
  self(owner)
{
}

ExptDriver::ExptImpl::~ExptImpl() {
}

void ExptDriver::ExptImpl::serialize(ostream& os, IO::IOFlag /*flag*/) const {

  if (doesDoUponCompletionExist()) {
	 Tcl_ResetResult(itsInterp);
	 int tclresult =
		Tcl_GlobalEval(itsInterp,
							"info body Expt::doUponCompletion");
	 
	 if (tclresult != TCL_OK) {
		throw OutputError("couldn't get the proc body for Expt::doUponCompletion");
	 }
	 
	 itsDoUponCompletionBody = Tcl_GetStringResult(itsInterp);
  }
  else {
	 itsDoUponCompletionBody = "";
  }

  os << itsDoUponCompletionBody.length() << endl
	  << itsDoUponCompletionBody << endl;
}

void ExptDriver::ExptImpl::deserialize(istream& is, IO::IOFlag /*flag*/) {
  int length;
  is >> length;
  if (is.peek() == '\n') { is.get(); }

  vector<char> buf(length+1);
  is.read(&buf[0], length);
  buf[length] = '\0';

  itsDoUponCompletionBody = &buf[0];

  string proc_cmd = "namespace eval Expt { proc doUponCompletion {} {"
	 + itsDoUponCompletionBody + "} }";

  vector<char> cmd_copy(proc_cmd.begin(), proc_cmd.end());
  cmd_copy.push_back('\0');
 
  int tclresult = Tcl_GlobalEval(itsInterp, &cmd_copy[0]);

  if (tclresult != TCL_OK) {
	 throw InputError("error while reconstituting Expt::doUponCompletion");
  }
}

bool ExptDriver::ExptImpl::doesDoUponCompletionExist() const {
  Tcl_ResetResult(itsInterp);
  int tclresult =
	 Tcl_GlobalEval(itsInterp,
						 "llength [  namespace eval ::Expt "
						 "            {info procs doUponCompletion}  ]");

  if (tclresult != TCL_OK) {
	 raiseBackgroundError("error getting procs in doesDoUponCompletionExist");
	 return false;
  }

  int llength;
  tclresult = Tcl_GetIntFromObj(itsInterp,
										  Tcl_GetObjResult(itsInterp),
										  &llength);

  if (tclresult != TCL_OK) {
	 raiseBackgroundError("error reading result in doesDoUponCompletionExist");
	 return false;
  }

  return (llength > 0);
}

void ExptDriver::ExptImpl::raiseBackgroundError(const char* msg) const {
  Tcl_AppendResult(itsInterp, msg, (char*) 0);
  Tcl_BackgroundError(itsInterp);
}

void ExptDriver::ExptImpl::raiseBackgroundError(const string& msg) const {
  Tcl_AppendResult(itsInterp, msg.c_str(), (char*) 0);
  Tcl_BackgroundError(itsInterp);
}

void ExptDriver::ExptImpl::doAutosave() {
  try {
	 DebugEvalNL(self->getAutosaveFile().c_str());
	 self->write(self->getAutosaveFile().c_str());
  }
  catch (TclError& err) {
	 raiseBackgroundError(err.msg().c_str());
  }
}

///////////////////////////////////////////////////////////////////////
//
// ExptDriver creator method definitions
//
///////////////////////////////////////////////////////////////////////

ExptDriver::ExptDriver() :
  itsAutosaveFile("__autosave_file"),
  itsBlockId(0),
  itsRhId(0),
  itsThId(0),
  itsImpl(new ExptImpl(this))
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
//   Tcl_Release(itsImpl->itsInterp);
  delete itsImpl; 
}

void ExptDriver::init() {
DOTRACE("ExptDriver::init");

  int result;

  // Get the current time/date
  static TclEvalCmd dateStringCmd("clock format [clock seconds]");

  result = dateStringCmd.invoke(itsImpl->itsInterp);
  if (result != TCL_OK) throw TclError("couldn't get date");

  itsBeginDate = Tcl_GetStringResult(itsImpl->itsInterp);

  // Get the hostname
  itsHostname = Tcl_GetVar2(itsImpl->itsInterp, "env", "HOST",
									 TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG);;

  // Get the subject's initials as the tail of the current directory
  static TclEvalCmd subjectKeyCmd("file tail [pwd]");

  result = subjectKeyCmd.invoke(itsImpl->itsInterp);
  if (result != TCL_OK) throw TclError("couldn't get subject initials");

  // Get the result, and remove an optional leading 'human_', if present
  const char* key = Tcl_GetStringResult(itsImpl->itsInterp);
  if ( strncmp(key, "human_", 6) == 0 ) {
    key += 6;
  }

  itsSubject = key;

  itsAutosaveFile = "__autosave_file";
  DebugEvalNL(itsAutosaveFile);
}

void ExptDriver::serialize(ostream &os, IOFlag flag) const {
DOTRACE("ExptDriver::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  ObjList::   theObjList()   .serialize(os, flag);
  PosList::   thePosList()   .serialize(os, flag);
  Tlist::     theTlist()     .serialize(os, flag);
  RhList::    theRhList()    .serialize(os, flag);
  ThList::    theThList()    .serialize(os, flag);
  BlockList:: theBlockList() .serialize(os, flag);

  os << itsHostname       << '\n';
  os << itsSubject        << '\n';
  os << itsBeginDate      << '\n';
  os << itsEndDate        << '\n';
  os << itsAutosaveFile   << '\n';

  os << itsBlockId << sep
	  << itsRhId << sep 
	  << itsThId << endl;

  itsImpl->serialize(os, flag);

  if (os.fail()) throw OutputError(ioTag);
}

void ExptDriver::deserialize(istream &is, IOFlag flag) {
DOTRACE("ExptDriver::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  ObjList::   theObjList()   .deserialize(is, flag);
  PosList::   thePosList()   .deserialize(is, flag);
  Tlist::     theTlist()     .deserialize(is, flag);
  RhList::    theRhList()    .deserialize(is, flag);
  ThList::    theThList()    .deserialize(is, flag);
  BlockList:: theBlockList() .deserialize(is, flag);

  getline(is, itsHostname,     '\n');
  getline(is, itsSubject,      '\n');
  getline(is, itsBeginDate,    '\n');
  getline(is, itsEndDate,      '\n');
  getline(is, itsAutosaveFile, '\n');

  is >> itsBlockId >> itsRhId >> itsThId;

  itsImpl->deserialize(is, flag);

  if (is.fail()) throw InputError(ioTag);
}

int ExptDriver::charCount() const {
DOTRACE("ExptDriver::charCount");
  return (0
			 + (ObjList::   theObjList()   .charCount()) + 1
			 + (PosList::   thePosList()   .charCount()) + 1
			 + (Tlist::     theTlist()     .charCount()) + 1
			 + (RhList::    theRhList()    .charCount()) + 1
			 + (ThList::    theThList()    .charCount()) + 1
			 + (BlockList:: theBlockList() .charCount()) + 1
			 + itsHostname.length() + 1
			 + itsSubject.length() + 1
			 + itsBeginDate.length() + 1
			 + itsEndDate.length() + 1
			 + itsAutosaveFile.length() + 1
			 + gCharCount<int>(itsBlockId) + 1
			 + gCharCount<int>(itsRhId) + 1
			 + gCharCount<int>(itsThId) + 1
			 + 5); // fudge factor
}

void ExptDriver::readFrom(Reader* reader) {
DOTRACE("ExptDriver::readFrom");
}

void ExptDriver::writeTo(Writer* writer) const {
DOTRACE("ExptDriver::writeTo");
}


///////////////////////////////////////////////////////////////////////
//
// ExptDriver accessor + manipulator method definitions
//
///////////////////////////////////////////////////////////////////////

Tcl_Interp* ExptDriver::getInterp() {
DOTRACE("ExptDriver::getInterp");
  return itsImpl->itsInterp;
}


void ExptDriver::setInterp(Tcl_Interp* interp) {
DOTRACE("ExptDriver::setInterp");
  // itsImpl->itsInterp can only be set once
  if (itsImpl->itsInterp == NULL) {
	 itsImpl->itsInterp = interp;
	 Tcl_Preserve(itsImpl->itsInterp);
  }
}

Block& ExptDriver::block() const {
DOTRACE("ExptDriver::block");  
  try {
#ifdef LOCAL_DEBUG
	 Block* block = BlockList::theBlockList().getCheckedPtr(itsBlockId);
	 DebugEvalNL((void *) block);
	 return *block;
#else
	 return *(BlockList::theBlockList().getCheckedPtr(itsBlockId));
#endif
  }
  catch (InvalidIdError& err) {
	 itsImpl->raiseBackgroundError(err.msg().c_str());
  }
}

ResponseHandler& ExptDriver::responseHandler() const {
DOTRACE("ExptDriver::responseHandler");
  try {
#ifdef LOCAL_DEBUG
	 ResponseHandler* rh = RhList::theRhList().getCheckedPtr(itsRhId);
	 DebugEval(itsRhId);   DebugEvalNL((void *) rh);
	 return *rh;
#else
	 return *(RhList::theRhList().getCheckedPtr(itsRhId));
#endif
  }
  catch (InvalidIdError& err) {
	 itsImpl->raiseBackgroundError(err.msg().c_str());
  }  
}

TimingHdlr& ExptDriver::timingHdlr() const {
DOTRACE("ExptDriver::timingHdlr");
  try {
#ifdef LOCAL_DEBUG
	 TimingHdlr* th = ThList::theThList().getCheckedPtr(itsThId);
	 DebugEvalNL((void *) th);
	 return *th;
#else
	 return *(ThList::theThList().getCheckedPtr(itsThId));
#endif
  }
  catch (InvalidIdError& err) {
	 itsImpl->raiseBackgroundError(err.msg().c_str());
  }  
}

//---------------------------------------------------------------------
//
// ExptDriver::assertIds --
//
// This function checks that the ExptDriver's block id, response
// handler id, and timing handler id, are all valid. If they all are,
// the function returns true as quickly as possible. If one or more of
// the id's is invalid, the function 1) generates a background error
// in the ExptDriver's Tcl_Interp, 2) sends a 'halt' message to any of
// the participants for which the id *is* valid, and 3) returns false.
//
//---------------------------------------------------------------------

inline bool ExptDriver::assertIds() const {
DOTRACE("ExptDriver::assertIds");
  if ( BlockList::theBlockList().isValidId(itsBlockId) &&
		 RhList::theRhList().isValidId(itsRhId) &&
		 ThList::theThList().isValidId(itsThId) ) {
	 return true;
  }

  // ...one of the validity checks failed, so generate an error+message
  itsImpl->raiseBackgroundError("invalid item id");

  // ...and halt any of the participants for which we have valid id's
  edHaltExpt();

  return false;
}

const string& ExptDriver::getAutosaveFile() const {
DOTRACE("ExptDriver::getAutosaveFile");
  return itsAutosaveFile;
}

void ExptDriver::setAutosaveFile(const string& str) {
DOTRACE("ExptDriver::setAutosaveFile");
  itsAutosaveFile = str;
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
  if ( !assertIds() ) return false;

  int period = timingHdlr().getAutosavePeriod();
  return ( (period > 0) && 
			  ((block().numCompleted() % period) == 0) &&
			  !(itsAutosaveFile.empty()) );
}

///////////////////////////////////////////////////////////////////////
//
// ExptDriver action method definitions
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//
// ExptDriver graphics-related callbacks
//
//---------------------------------------------------------------------

void ExptDriver::draw() {
DOTRACE("ExptDriver::draw");
  if ( !assertIds() ) return;

  try {
	 block().drawTrial();
  }
  catch (InvalidIdError& err) {
	 itsImpl->raiseBackgroundError(err.msg().c_str());
  }
}

void ExptDriver::undraw() {
DOTRACE("ExptDriver::undraw");
  if ( !assertIds() ) return;

  try {
	 block().undrawTrial();
  }
  catch (InvalidIdError& err) {
	 itsImpl->raiseBackgroundError(err.msg().c_str());
  }
}

void ExptDriver::edSwapBuffers() {
DOTRACE("ExptDriver::edSwapBuffers");
  try { 
	 ObjTogl::theToglConfig()->swapBuffers();
  }
  catch (TclError& err) {
	 itsImpl->raiseBackgroundError(err.msg().c_str());
  }
}

//---------------------------------------------------------------------
//
// ExptDriver::edBeginExpt --
//
//---------------------------------------------------------------------

void ExptDriver::edBeginExpt() {
DOTRACE("ExptDriver::edBeginExpt");

  init(); 

  gettimeofday(&itsBeginTime, NULL);

  edBeginTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::edBeginTrial --
//
//--------------------------------------------------------------------

void ExptDriver::edBeginTrial() {
DOTRACE("ExptDriver::edBeginTrial");
  if ( !assertIds() ) return;

  if (block().isComplete()) return;
  
  TimeTraceNL("beginTrial", timingHdlr().getElapsedMsec());
  
  Trial& trial = block().getCurTrial();
  
  itsThId = trial.getTimingHdlr();       DebugEval(itsThId);
  itsRhId = trial.getResponseHandler();  DebugEval(itsRhId);
  
  if ( !assertIds() ) return;

  block().beginTrial();
  timingHdlr().thBeginTrial();
  responseHandler().rhBeginTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::edResponseSeen --
//
// This procedure is basically a hook for ResponseHdlr's to quickly
// notify ExptDriver that a response has been seen, before they go on
// and process the response. This allows TimingHdlr's to more
// accurately schedule their FROM_RESPONSE timers.
//
//--------------------------------------------------------------------

void ExptDriver::edResponseSeen() {
DOTRACE("ExptDriver::edResponseSeen");
  if ( !assertIds() ) return;

  TimeTraceNL("edResponseSeen", timingHdlr().getElapsedMsec());

  timingHdlr().thResponseSeen();
}

//--------------------------------------------------------------------
//
// ExptDriver::edProcessResponse --
//
// This function assumes that it is passed a valid response (although
// an invalid response should only mess up semantics, not cause a
// crash), and instructs the current Block to record the response.
//
//--------------------------------------------------------------------

void ExptDriver::edProcessResponse(int response) {
DOTRACE("ExptDriver::edProcessResponse");
  if ( !assertIds() ) return;

  TimeTraceNL("edProcessResponse", timingHdlr().getElapsedMsec());

  block().processResponse(response);
}

//--------------------------------------------------------------------
//
// ExptDriver::edAbortTrial --
//
//--------------------------------------------------------------------

void ExptDriver::edAbortTrial() {
DOTRACE("ExptDriver::edAbortTrial");
  if ( !assertIds() ) return;

  TimeTraceNL("edAbortTrial", timingHdlr().getElapsedMsec());
  
  responseHandler().rhAbortTrial();
  block().abortTrial();
  timingHdlr().thAbortTrial();
}

//---------------------------------------------------------------------
//
// ExptDriver::edEndTrial --
//
//---------------------------------------------------------------------

void ExptDriver::edEndTrial() {
DOTRACE("ExptDriver::edEndTrial");
  if ( !assertIds() ) return;

  TimeTraceNL("edEndTrial", timingHdlr().getElapsedMsec());
  
  block().endTrial();

  if ( needAutosave() ) {
	 itsImpl->doAutosave();
  }

  DebugEval(block().numCompleted());
  DebugEval(block().numTrials());
  DebugEvalNL(block().isComplete());

  if (block().isComplete()) {
	 BlockList& blist = BlockList::theBlockList();

	 // This increments itsBlockId until we have either run out of
	 // Block's, or found the next valid Block
	 while ( (++itsBlockId < blist.capacity()) 
				&& !blist.isValidId(itsBlockId) ) { /* empty loop body */ }

	 // If we've found a new valid Block, then we don't need to do
	 // anything here since the new Block will automatically be used
	 // when edBeginTrial is next called.
	 if (blist.isValidId(itsBlockId)) { /* do nothing */ }

	 // ...Otherwise we've run out of Block's, so write the ExptDriver
	 // and quit.
	 else {
		int elapsed_msec = elapsedMsecSince(itsBeginTime);
		cout << "expt completed in " << elapsed_msec << " milliseconds\n";

		// Write but don't exit
		ExptDriver::writeAndExit(false);

		// Call the user-defined callback
		if (itsImpl->doesDoUponCompletionExist()) {
		  int tclresult = Tcl_GlobalEval(itsImpl->itsInterp, "Expt::doUponCompletion");
		  if (tclresult != TCL_OK) {
			 itsImpl->raiseBackgroundError(
					         "error while evaluating Expt::doUponCompletion");
		  }
		}

		return;
	 }
  }    

  edBeginTrial();
}

//--------------------------------------------------------------------
//
// ExptDriver::edHaltExpt --
//
//--------------------------------------------------------------------

void ExptDriver::edHaltExpt() const {
DOTRACE("ExptDriver::edHaltExpt");

  if ( ThList::theThList().isValidId(itsThId) ) { 
	 TimeTraceNL("edHaltExpt", timingHdlr().getElapsedMsec());
  }

  if ( BlockList::theBlockList().isValidId(itsBlockId) ) {
	 block().haltExpt();
  }
  if ( RhList::theRhList().isValidId(itsRhId) ) {
	 responseHandler().rhHaltExpt();
  }
  if ( ThList::theThList().isValidId(itsThId) ) {
	 timingHdlr().thHaltExpt();
  }
}

//---------------------------------------------------------------------
//
// ExptDriver::edResetExpt() --
//
//---------------------------------------------------------------------

void ExptDriver::edResetExpt() {
DOTRACE("ExptDriver::edResetExpt");
  edHaltExpt();

  while (1) {
	 if ( BlockList::theBlockList().isValidId(itsBlockId) ) {
		block().resetBlock();
	 }

	 if (itsBlockId > 0) { --itsBlockId; }
	 else return;
  }
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
//--------------------------------------------------------------------

void ExptDriver::writeAndExit(bool quitApplication) {
DOTRACE("ExptDriver::writeAndExit");
  Assert(itsImpl->itsInterp != 0);

  edHaltExpt();

  // Get the current date/time and record it as the ending date for
  // the ExptDriver.
  static TclEvalCmd dateStringCmd("clock format [clock seconds]");

  int result = dateStringCmd.invoke(itsImpl->itsInterp);
  if (result != TCL_OK) 
	 { itsImpl->raiseBackgroundError("error getting date string"); }
  itsEndDate = Tcl_GetStringResult(itsImpl->itsInterp);

  // Format the current time into a unique filename extension
  static TclEvalCmd uniqueFilenameCmd(
        "clock format [clock seconds] -format %H%M%d%b%Y");

  result = uniqueFilenameCmd.invoke(itsImpl->itsInterp);
  if (result != TCL_OK)
	 { itsImpl->raiseBackgroundError("error creating unique filenames"); }
  string unique_filename = Tcl_GetStringResult(itsImpl->itsInterp);

  // Write the main experiment file
  string expt_filename = string("expt") + unique_filename;

  try {
	 write(expt_filename.c_str());
	 cout << "wrote file " << expt_filename << endl;

	 // Write the responses file
	 string resp_filename = string("resp") + unique_filename;
	 TlistTcl::writeResponsesProc(resp_filename.c_str());
	 cout << "wrote file " << resp_filename << endl;

  }
  catch (IoError& err) {
	 itsImpl->raiseBackgroundError(err.msg().c_str());
  }
  catch (TclError& err) {
	 itsImpl->raiseBackgroundError(err.msg().c_str());
  }

  // Gracefully exit the application (or not)
  if (quitApplication) {
	 Tcl_Exit(0);
  }
}

static const char vcid_exptdriver_cc[] = "$Header$";
#endif // !EXPTDRIVER_CC_DEFINED
