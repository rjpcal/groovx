///////////////////////////////////////////////////////////////////////
//
// exptdriver.cc
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Mon Oct 23 18:16:24 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_CC_DEFINED
#define EXPTDRIVER_CC_DEFINED

#include "exptdriver.h"

#include "block.h"
#include "tlistutils.h"
#include "objlist.h"
#include "objtogl.h"
#include "toglconfig.h"
#include "poslist.h"
#include "blocklist.h"
#include "rhlist.h"
#include "thlist.h"
#include "tlist.h"
#include "tlistwidget.h"
#include "trialbase.h"

#include "io/reader.h"
#include "io/asciistreamwriter.h"

#include "system/system.h"

#include "tcl/tclerror.h"
#include "tcl/tclevalcmd.h"

#include "util/arrays.h"
#include "util/errorhandler.h"
#include "util/stopwatch.h"
#include "util/strings.h"

#include <tcl.h>
#include <iostream.h>
#include <fstream.h>
#include <sys/time.h>

#define DYNAMIC_TRACE_EXPR ExptDriver::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

#define TIME_TRACE

Util::Tracer ExptDriver::tracer;

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {

  const IO::VersionId EXPTDRIVER_SERIAL_VERSION_ID = 3;

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
// ExptDriver::Impl utility class
//
///////////////////////////////////////////////////////////////////////

class ExptDriver::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(int argc, char** argv, ExptDriver* owner, Tcl_Interp* interp);
  ~Impl();

  //////////////////////
  // helper functions //
  //////////////////////

private:
  bool doesDoUponCompletionExist() const;

  void updateDoUponCompletionBody() const;

  void recreateDoUponCompletionProc() const;

  void doAutosave();

  // This accessor will NOT recover if the corresponding id is
  // invalid; clients must check validity before calling the accessor.
  Block& block() const;

  // Check the validity of all its id's, return true if all are ok,
  // otherwise returns false, halts the experiment, and issues an
  // error message to itsInterp.
  bool assertIds() const;

  bool needAutosave() const;

  // Switches itsBlockId to the next valid block and returns true, or
  // returns false if there are no more valid blocks.
  bool gotoNextValidBlock();

  bool safeTclGlobalEval(const char* script) const;

  void doUponCompletion() const;

  void noteElapsedTime() const;

  void getCurrentTimeDateString(fixed_string& date_out) const;

  void getHostname(fixed_string& hostname_out) const;

  void getSubjectKey(fixed_string& subjectkey_out) const;

  dynamic_string makeUniqueFileExtension() const;

  //////////////////////////
  // ExptDriver delegands //
  //////////////////////////

public:

  IO::VersionId serialVersionId() const
	 { return EXPTDRIVER_SERIAL_VERSION_ID; }

  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;

  void manageObject(const char* name, IO::IoObject* object);

  void init();

  const fixed_string& getAutosaveFile() const { return itsAutosaveFile; }

  void setAutosaveFile(const fixed_string& str) { itsAutosaveFile = str; }

  int getAutosavePeriod() const { return itsAutosavePeriod; }

  void setAutosavePeriod(int period) { itsAutosavePeriod = period; }

  const char* getInfoLog() const
    { return itsInfoLog.c_str(); }

  void addLogInfo(const char* message)
  {
	 fixed_string date_string;
	 getCurrentTimeDateString(date_string);

	 itsInfoLog.append("@");
	 itsInfoLog.append(date_string);
	 itsInfoLog.append(" ");
	 itsInfoLog.append(message);
	 itsInfoLog.append("\n");
  }

  Util::ErrorHandler& getErrorHandler()
	 { return itsErrHandler; }

  GWT::Widget* getWidget()
	 { return ObjTogl::theToglConfig(); }

  GWT::Canvas* getCanvas()
	 { return getWidget()->getCanvas(); }

  void edBeginExpt();
  void edEndTrial();
  void edNextBlock();
  void edHaltExpt() const;
  void edResumeExpt();
  void edResetExpt();
  void edSetCurrentTrial(int trial);

  void edEndExpt();

  void writeASW(const char* filename) const;

  void storeData();

  //////////////////
  // data members //
  //////////////////

private:
  ExptDriver* itsOwner;

  Tcl_Interp* itsInterp;

  fixed_string itsHostname;	  // Host computer on which Expt was begun
  fixed_string itsSubject;		  // Id of subject on whom Expt was performed
  fixed_string itsBeginDate;	  // Date(+time) when Expt was begun
  fixed_string itsEndDate;		  // Date(+time) when Expt was stopped
  fixed_string itsAutosaveFile; // Filename used for autosaves

  dynamic_string itsInfoLog;

  int itsAutosavePeriod;

  int itsBlockId;
  int itsDummyRhId;
  int itsDummyThId;

  mutable StopWatch itsTimer;

  mutable dynamic_string itsDoUponCompletionBody;

  mutable Tcl::BkdErrorHandler itsErrHandler;

//   struct ManagedObject {
// 	 ManagedObject(const string& n, IO::IoObject* obj) :
// 		name(n), object(obj) {}
// 	 ManagedObject(const char* n, IO::IoObject* obj) :
// 		name(n), object(obj) {}
// 	 string name;
// 	 IO::IoObject* object;
//   };

//   vector<ManagedObject> itsManagedObjects;
};

///////////////////////////////////////////////////////////////////////
//
// ExptDriver::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

ExptDriver::Impl::Impl(int argc, char** argv,
							  ExptDriver* owner, Tcl_Interp* interp) :
  itsOwner(owner),
  itsInterp(interp),
  itsHostname(""),
  itsSubject(""),
  itsBeginDate(""),
  itsEndDate(""),
  itsAutosaveFile("__autosave_file"),
  itsInfoLog(),
  itsAutosavePeriod(10),
  itsBlockId(0),
  itsDummyRhId(0),
  itsDummyThId(0),
  itsTimer(),
  itsDoUponCompletionBody(),
  itsErrHandler(interp)
  //  ,itsManagedObjects()
{
DOTRACE("ExptDriver::Impl::Impl");

  dynamic_string cmd_line("command line: ");

  for (int i = 0; i < argc; ++i) {
	 cmd_line.append(argv[i]);
	 cmd_line.append(" ");
  }

  addLogInfo(cmd_line.c_str());

  Tcl_Preserve(itsInterp);
}

ExptDriver::Impl::~Impl() {
DOTRACE("ExptDriver::Impl::~Impl");
//   Tcl_Release(itsInterp);
}

bool ExptDriver::Impl::doesDoUponCompletionExist() const {
DOTRACE("ExptDriver::Impl::doesDoUponCompletionExist");
  Tcl_ResetResult(itsInterp);
  int tclresult =
	 Tcl_GlobalEval(itsInterp,
						 "llength [  namespace eval ::Expt "
						 "            {info procs doUponCompletion}  ]");

  if (tclresult != TCL_OK) {
	 itsErrHandler.handleMsg("error getting procs in doesDoUponCompletionExist");
	 return false;
  }

  int llength;
  tclresult = Tcl_GetIntFromObj(itsInterp,
										  Tcl_GetObjResult(itsInterp),
										  &llength);

  if (tclresult != TCL_OK) {
	 itsErrHandler.handleMsg("error reading result in doesDoUponCompletionExist");
	 return false;
  }

  return (llength > 0);
}

void ExptDriver::Impl::updateDoUponCompletionBody() const {
DOTRACE("ExptDriver::Impl::updateDoUponCompletionBody");
  if (doesDoUponCompletionExist()) {
	 Tcl_ResetResult(itsInterp);
	 int tclresult =
		Tcl_GlobalEval(itsInterp,
							"info body Expt::doUponCompletion");

	 if (tclresult != TCL_OK) {
		throw IO::OutputError("couldn't get the proc body for Expt::doUponCompletion");
	 }

	 itsDoUponCompletionBody = Tcl_GetStringResult(itsInterp);
	 Tcl_ResetResult(itsInterp);
  }
  else {
	 itsDoUponCompletionBody = "";
  }
}

void ExptDriver::Impl::recreateDoUponCompletionProc() const {
DOTRACE("ExptDriver::Impl::recreateDoUponCompletionProc");
  try {
	 dynamic_string proc_cmd_str =
		"namespace eval Expt { proc doUponCompletion {} {";
	 proc_cmd_str += itsDoUponCompletionBody;
	 proc_cmd_str += "} }";

	 Tcl::TclEvalCmd proc_cmd(proc_cmd_str.c_str(),
									  Tcl::TclEvalCmd::THROW_EXCEPTION);
	 proc_cmd.invoke(itsInterp);
  }
  catch (Tcl::TclError& err) {
	 throw IO::InputError(err.msg_cstr());
  }
}

void ExptDriver::Impl::doAutosave() {
DOTRACE("ExptDriver::Impl::doAutosave");
  try {
	 DebugEvalNL(getAutosaveFile().c_str());
	 writeASW(getAutosaveFile().c_str());
  }
  catch (Tcl::TclError& err) {
	 itsErrHandler.handleMsg(err.msg_cstr());
  }
}

Block& ExptDriver::Impl::block() const {
DOTRACE("ExptDriver::Impl::block");

  Precondition( BlockList::theBlockList().isValidId(itsBlockId) );

#ifdef LOCAL_DEBUG
  BlockList::Ptr block = BlockList::theBlockList().getPtr(itsBlockId);
  DebugEvalNL((void *) block);
  return *block;
#else
  return *(BlockList::theBlockList().getPtr(itsBlockId));
#endif
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

inline bool ExptDriver::Impl::assertIds() const {
DOTRACE("ExptDriver::Impl::assertIds");
  if ( BlockList::theBlockList().isValidId(itsBlockId) ) {
	 return true;
  }

  // ...one of the validity checks failed, so generate an error+message
  itsErrHandler.handleMsg("invalid Block id in ExptDriver::assertIds()");

  // ...and halt any of the participants for which we have valid id's
  edHaltExpt();

  return false;
}

//--------------------------------------------------------------------
//
// ExptDriver::needAutosave --
//
// Determine whether an autosave is necessary now. An autosave is
// requested only if the autosave period is positive, and the number
// of completed trials is evenly divisible by the autosave period.
//
//--------------------------------------------------------------------

bool ExptDriver::Impl::needAutosave() const {
DOTRACE("ExptDriver::Impl::needAutosave");
  if ( !assertIds() ) return false;

  return ( (itsAutosavePeriod > 0) &&
			  ((block().numCompleted() % itsAutosavePeriod) == 0) &&
			  !(itsAutosaveFile.empty()) );
}

bool ExptDriver::Impl::gotoNextValidBlock() {
DOTRACE("ExptDriver::Impl::gotoNextValidBlock");
  BlockList& blist = BlockList::theBlockList();

  // This increments itsImpl->itsBlockId until we have either run out of
  // Block's, or found the next valid Block
  while ( (++itsBlockId < blist.capacity())
			 && !blist.isValidId(itsBlockId) ) { /* empty loop body */ }

  return blist.isValidId(itsBlockId);
}

bool ExptDriver::Impl::safeTclGlobalEval(const char* script) const {
DOTRACE("ExptDriver::Impl::safeTclGlobalEval");
  fixed_string temp_buf = script;

  int tclresult = Tcl_GlobalEval(itsInterp, temp_buf.data());

  if (tclresult == TCL_OK) return true;

  // else...
  dynamic_string msg = "error while evaluating "; msg += script;
  itsErrHandler.handleMsg(msg.c_str());
  return false;
}

void ExptDriver::Impl::doUponCompletion() const {
DOTRACE("ExptDriver::Impl::doUponCompletion");
  if (doesDoUponCompletionExist()) {
	 safeTclGlobalEval("Expt::doUponCompletion");
  }
}

void ExptDriver::Impl::noteElapsedTime() const {
DOTRACE("ExptDriver::Impl::noteElapsedTime");
  cout << "expt completed in "
		 << itsTimer.elapsedMsec()
		 << " milliseconds\n";
}

void ExptDriver::Impl::getCurrentTimeDateString(fixed_string& date_out) const {
DOTRACE("ExptDriver::Impl::getCurrentTimeDateString");
  static Tcl::TclEvalCmd dateStringCmd("clock format [clock seconds]",
													Tcl::TclEvalCmd::THROW_EXCEPTION);

  dateStringCmd.invoke(itsInterp);
  date_out = Tcl_GetStringResult(itsInterp);
}

void ExptDriver::Impl::getHostname(fixed_string& hostname_out) const {
DOTRACE("ExptDriver::Impl::getHostname");
  char* temp = Tcl_GetVar2(itsInterp, "env", "HOST",
									TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG);;

  if (temp == 0) { throw Tcl::TclError(); }
  hostname_out = temp;
}

void ExptDriver::Impl::getSubjectKey(fixed_string& subjectkey_out) const {
DOTRACE("ExptDriver::Impl::getSubjectKey");

  // Get the subject's initials as the tail of the current directory
  static Tcl::TclEvalCmd subjectKeyCmd("file tail [pwd]",
													Tcl::TclEvalCmd::THROW_EXCEPTION);

  subjectKeyCmd.invoke(itsInterp);

  // Get the result, and remove an optional leading 'human_', if present
  const char* key = Tcl_GetStringResult(itsInterp);
  if ( strncmp(key, "human_", 6) == 0 ) {
    key += 6;
  }

  subjectkey_out = key;
}

dynamic_string ExptDriver::Impl::makeUniqueFileExtension() const {
DOTRACE("ExptDriver::Impl::makeUniqueFileExtension");

  // Format the current time into a unique filename extension
  static Tcl::TclEvalCmd uniqueFilenameCmd(
        "clock format [clock seconds] -format %H%M%d%b%Y",
		  Tcl::TclEvalCmd::THROW_EXCEPTION);

  static dynamic_string previous_result = "";
  static char tag[2] = {'a', '\0'};

  uniqueFilenameCmd.invoke(itsInterp);

  dynamic_string current_result = Tcl_GetStringResult(itsInterp);

  if (current_result.equals(previous_result)) {
	 ++tag[0];
	 if (tag[0] > 'z') tag[0] = 'a';

	 current_result.append(tag);
  }
  else {
	 previous_result = current_result;
	 tag[0] = 'a';
  }

  return current_result;
}

///////////////////////////////////////////////////////////////////////
//
// ExptDriver::Impl delegand functions
//
///////////////////////////////////////////////////////////////////////

void ExptDriver::Impl::readFrom(IO::Reader* reader) {
DOTRACE("ExptDriver::Impl::readFrom");

  IO::VersionId svid = reader->readSerialVersionId();

  if (svid < 3)
	 {
		reader->readOwnedObject("theObjList", &ObjList::theObjList());
		reader->readOwnedObject("thePosList", &PosList::thePosList());
		reader->readOwnedObject("theTlist", &Tlist::theTlist());
		reader->readOwnedObject("theRhList", &RhList::theRhList());
		reader->readOwnedObject("theThList", &ThList::theThList());
	 }

  reader->readOwnedObject("theBlockList", &BlockList::theBlockList());

  reader->readValue("hostname", itsHostname);
  reader->readValue("subject", itsSubject);
  reader->readValue("beginDate", itsBeginDate);
  reader->readValue("endDate", itsEndDate);
  reader->readValue("autosaveFile", itsAutosaveFile);

  if (svid >= 1)
	 reader->readValue("autosavePeriod", itsAutosavePeriod);

  if (svid >= 2)
	 reader->readValue("infoLog", itsInfoLog);

  reader->readValue("blockId", itsBlockId);
  reader->readValue("rhId", itsDummyRhId);
  reader->readValue("thId", itsDummyThId);

  reader->readValue("doUponCompletionScript", itsDoUponCompletionBody);
  recreateDoUponCompletionProc();
}

void ExptDriver::Impl::writeTo(IO::Writer* writer) const {
DOTRACE("ExptDriver::Impl::writeTo");

  if (EXPTDRIVER_SERIAL_VERSION_ID < 3)
	 {
		writer->writeOwnedObject("theObjList", &ObjList::theObjList());
		writer->writeOwnedObject("thePosList", &PosList::thePosList());
		writer->writeOwnedObject("theTlist", &Tlist::theTlist());
		writer->writeOwnedObject("theRhList", &RhList::theRhList());
		writer->writeOwnedObject("theThList", &ThList::theThList());
	 }

  writer->writeOwnedObject("theBlockList", &BlockList::theBlockList());

  writer->writeValue("hostname", itsHostname);
  writer->writeValue("subject", itsSubject);
  writer->writeValue("beginDate", itsBeginDate);
  writer->writeValue("endDate", itsEndDate);
  writer->writeValue("autosaveFile", itsAutosaveFile);

  if (EXPTDRIVER_SERIAL_VERSION_ID >= 1)
	 writer->writeValue("autosavePeriod", itsAutosavePeriod);

  if (EXPTDRIVER_SERIAL_VERSION_ID >= 2)
	 writer->writeValue("infoLog", itsInfoLog);

  writer->writeValue("blockId", itsBlockId);
  writer->writeValue("rhId", itsDummyRhId);
  writer->writeValue("thId", itsDummyThId);

  updateDoUponCompletionBody();
  writer->writeValue("doUponCompletionScript", itsDoUponCompletionBody);
}

void ExptDriver::Impl::manageObject(const char* /* name */, IO::IoObject* /* object */) {
DOTRACE("ExptDriver::Impl::manageObject");
//   itsManagedObjects.push_back(ManagedObject(name, object));
}

void ExptDriver::Impl::init() {
DOTRACE("ExptDriver::Impl::init");

  getCurrentTimeDateString(itsBeginDate);
  getHostname(itsHostname);
  getSubjectKey(itsSubject);
}

///////////////////////////////////////////////////////////////////////
//
// ExptDriver action method definitions
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//
// ExptDriver::edBeginExpt --
//
//---------------------------------------------------------------------

void ExptDriver::Impl::edBeginExpt() {
DOTRACE("ExptDriver::Impl::edBeginExpt");

  addLogInfo("Beginning experiment.");

  init();

  itsTimer.restart();

  if ( !assertIds() ) return;

  block().beginTrial(*itsOwner);
}

//---------------------------------------------------------------------
//
// ExptDriver::edEndTrial --
//
//---------------------------------------------------------------------

void ExptDriver::Impl::edEndTrial() {
DOTRACE("ExptDriver::Impl::edEndTrial");

  if ( needAutosave() ) { doAutosave(); }
}

//---------------------------------------------------------------------
//
// ExptDriver::edNextBlock --
//
//---------------------------------------------------------------------

void ExptDriver::Impl::edNextBlock() {
DOTRACE("ExptDriver::Impl::edNextBlock");
  bool haveMoreBlocks = gotoNextValidBlock();

  if ( !haveMoreBlocks ) {
	 edEndExpt();
  }
  else {
	 block().beginTrial(*itsOwner);
  }
}

//--------------------------------------------------------------------
//
// ExptDriver::edHaltExpt --
//
//--------------------------------------------------------------------

void ExptDriver::Impl::edHaltExpt() const {
DOTRACE("ExptDriver::Impl::edHaltExpt");

  if ( BlockList::theBlockList().isValidId(itsBlockId) ) {
	 block().haltExpt();
  }
}

//--------------------------------------------------------------------
//
// ExptDriver::edResumeExpt --
//
//--------------------------------------------------------------------

void ExptDriver::Impl::edResumeExpt() {
DOTRACE("ExptDriver::Impl::edResumeExpt");
  if ( !assertIds() ) return;

  block().beginTrial(*itsOwner);
}

//---------------------------------------------------------------------
//
// ExptDriver::edResetExpt() --
//
//---------------------------------------------------------------------

void ExptDriver::Impl::edResetExpt() {
DOTRACE("ExptDriver::Impl::edResetExpt");
  edHaltExpt();

  while (1) {
	 if ( BlockList::theBlockList().isValidId(itsBlockId) ) {
		block().resetBlock();
	 }

	 if (itsBlockId > 0) { --itsBlockId; }
	 else return;
  }
}

//---------------------------------------------------------------------
//
// ExptDriver::edSetCurrentTrial() --
//
//---------------------------------------------------------------------

void ExptDriver::Impl::edSetCurrentTrial(int trial) {
DOTRACE("ExptDriver::Impl::edSetCurrentTrial");
  TlistWidget* widg =
	 dynamic_cast<TlistWidget*>(getWidget());

  if (widg != 0) {
	 widg->setCurTrial(NullableItemWithId<TrialBase>(trial));
  }
}

//---------------------------------------------------------------------
//
// ExptDriver::edEndExpt() --
//
//---------------------------------------------------------------------

void ExptDriver::Impl::edEndExpt() {
DOTRACE("ExptDriver::Impl::edEndExpt");
  noteElapsedTime();

  addLogInfo("Experiment complete.");

  storeData();
  doUponCompletion();		  // Call the user-defined callback
}

//--------------------------------------------------------------------
//
// ExptDriver::writeASW --
//
//--------------------------------------------------------------------

void ExptDriver::Impl::writeASW(const char* filename) const {
DOTRACE("ExptDriver::Impl::write");
  STD_IO::ofstream ofs(filename);
  if (ofs.fail()) throw IO::FilenameError(filename);
  AsciiStreamWriter writer(ofs);
  writer.writeRoot(itsOwner);
}

//--------------------------------------------------------------------
//
// ExptDriver::storeData --
//
// The experiment and a summary of the responses to it are written to
// files with unique filenames.
//
//--------------------------------------------------------------------

void ExptDriver::Impl::storeData() {
DOTRACE("ExptDriver::Impl::storeData");
  Precondition(itsInterp != 0);

  edHaltExpt();  

  try {
	 getCurrentTimeDateString(itsEndDate);

	 dynamic_string unique_file_extension = makeUniqueFileExtension();

	 // Write the main experiment file
	 dynamic_string expt_filename = "expt";
	 expt_filename += unique_file_extension;
	 expt_filename += ".asw";
	 writeASW(expt_filename.c_str());
	 cout << "wrote file " << expt_filename << endl;

	 // Write the responses file
	 dynamic_string resp_filename = "resp";
	 resp_filename += unique_file_extension;
	 TlistUtils::writeResponses(resp_filename.c_str());
	 cout << "wrote file " << resp_filename << endl;

	 // Change file access modes to allow read-only by all
	 System::mode_t datafile_mode = System::IRUSR | System::IRGRP | System::IROTH;

	 int error1 =
		System::theSystem().chmod(expt_filename.c_str(), datafile_mode);
	 int error2 =
		System::theSystem().chmod(resp_filename.c_str(), datafile_mode);
	 if (error1 != 0 || error2 != 0) {
		itsErrHandler.handleMsg("error during System::chmod");
		return;
	 }

	 addLogInfo("Experiment saved.");
  }
  catch (IO::IoError& err) {
	 itsErrHandler.handleMsg(err.msg_cstr());
	 return;
  }
  catch (Tcl::TclError& err) {
	 itsErrHandler.handleMsg(err.msg_cstr());
	 return;
  }
}




///////////////////////////////////////////////////////////////////////
//
// ExptDriver creator method definitions
//
///////////////////////////////////////////////////////////////////////

ExptDriver::ExptDriver(int argc, char** argv, Tcl_Interp* interp) :
  itsImpl(new Impl(argc, argv, this, interp))
{
DOTRACE("ExptDriver::ExptDriver");
}

ExptDriver::~ExptDriver() {
DOTRACE("ExptDriver::~ExptDriver");
  delete itsImpl;
}

IO::VersionId ExptDriver::serialVersionId() const
  { return itsImpl->serialVersionId(); }

void ExptDriver::readFrom(IO::Reader* reader)
  { itsImpl->readFrom(reader); }

void ExptDriver::writeTo(IO::Writer* writer) const
  { itsImpl->writeTo(writer); }


///////////////////////////////////////////////////////////////////////
//
// ExptDriver accessor + manipulator method definitions
//
///////////////////////////////////////////////////////////////////////

const fixed_string& ExptDriver::getAutosaveFile() const
  { return itsImpl->getAutosaveFile(); }

void ExptDriver::setAutosaveFile(const fixed_string& str)
  { itsImpl->setAutosaveFile(str); }

int ExptDriver::getAutosavePeriod() const
  { return itsImpl->getAutosavePeriod(); }

void ExptDriver::setAutosavePeriod(int period)
  { itsImpl->setAutosavePeriod(period); }

const char* ExptDriver::getInfoLog() const
  { return itsImpl->getInfoLog(); }

void ExptDriver::addLogInfo(const char* message)
  { itsImpl->addLogInfo(message); }

Util::ErrorHandler& ExptDriver::getErrorHandler()
  { return itsImpl->getErrorHandler(); }

GWT::Widget* ExptDriver::getWidget()
  { return itsImpl->getWidget(); }

GWT::Canvas* ExptDriver::getCanvas()
  { return itsImpl->getCanvas(); }

void ExptDriver::manageObject(const char* name, IO::IoObject* object)
  { itsImpl->manageObject(name, object); }

void ExptDriver::edBeginExpt()
  { itsImpl->edBeginExpt(); }

void ExptDriver::edEndTrial()
  { itsImpl->edEndTrial(); }

void ExptDriver::edNextBlock()
  { itsImpl->edNextBlock(); }

void ExptDriver::edHaltExpt() const
  { itsImpl->edHaltExpt(); }

void ExptDriver::edResumeExpt()
  { itsImpl->edResumeExpt(); }

void ExptDriver::edResetExpt()
  { itsImpl->edResetExpt(); }

void ExptDriver::edSetCurrentTrial(int trial)
  { itsImpl->edSetCurrentTrial(trial); }

void ExptDriver::storeData()
  { itsImpl->storeData(); }

static const char vcid_exptdriver_cc[] = "$Header$";
#endif // !EXPTDRIVER_CC_DEFINED
