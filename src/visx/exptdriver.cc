///////////////////////////////////////////////////////////////////////
//
// exptdriver.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 11 13:33:50 1999
// written: Wed Aug  8 20:16:40 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_CC_DEFINED
#define EXPTDRIVER_CC_DEFINED

#include "exptdriver.h"

#include "block.h"
#include "tlistutils.h"

#include "gwt/widget.h"

#include "io/ioutil.h"
#include "io/readutils.h"
#include "io/writeutils.h"

#include "system/system.h"

#include "tcl/tclcode.h"
#include "tcl/tclerror.h"
#include "tcl/tclsafeinterp.h"

#include "util/error.h"
#include "util/errorhandler.h"
#include "util/log.h"
#include "util/minivec.h"
#include "util/ref.h"
#include "util/stopwatch.h"
#include "util/strings.h"

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

namespace
{
  const IO::VersionId EXPTDRIVER_SERIAL_VERSION_ID = 3;

#ifdef TIME_TRACE
  inline void TimeTraceNL(const char* loc, int msec) {
    Util::log() << "in " << loc << ", elapsed time == " << msec << '\n';
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

  bool haveValidBlock() const
    {
      return ( itsCurrentBlockIdx < itsBlocks.size() );
    }

  // Ensure that there is a valid Block. If there is not, throw an
  // exception.
  void ensureHasBlock() const
    {
      if ( !haveValidBlock() )
        {
          throw Util::Error("the experiment must have at least one Block "
                            "before it can be started");
        }
    }

  bool needAutosave() const;
  void doAutosave();

  void doUponCompletion() const;

  //////////////////////////
  // ExptDriver delegands //
  //////////////////////////

public:

  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;

  void addLogInfo(const char* message)
  {
    fstring date_string = System::theSystem().formattedTime();

    itsInfoLog.append("@");
    itsInfoLog.append(date_string);
    itsInfoLog.append(" ");
    itsInfoLog.append(message);
    itsInfoLog.append("\n");
  }

  void addBlock(Ref<Block> block)
    { itsBlocks.push_back(block); }

  Ref<Block> currentBlock() const
    {
      if ( !haveValidBlock() )
        throw Util::Error("no current block exists");
      return itsBlocks.at(itsCurrentBlockIdx);
    }

  void edBeginExpt();
  void edEndTrial();
  void edNextBlock();
  void edHaltExpt() const;
  void edResumeExpt();
  void edClearExpt();
  void edResetExpt();

  void edEndExpt();

  void storeData();

  //////////////////
  // data members //
  //////////////////

private:
  ExptDriver* itsOwner;

  Tcl::Interp itsInterp;

public:
  WeakRef<GWT::Widget> itsWidget;

  fstring itsHostname;     // Host computer on which Expt was begun
  fstring itsSubject;      // Id of subject on whom Expt was performed
  fstring itsBeginDate;    // Date(+time) when Expt was begun
  fstring itsEndDate;      // Date(+time) when Expt was stopped
  fstring itsAutosaveFile; // Filename used for autosaves

  fstring itsInfoLog;

  int itsAutosavePeriod;

private:
  minivec<Ref<Block> > itsBlocks;

  unsigned int itsCurrentBlockIdx;

  mutable StopWatch itsTimer;

public:
  mutable Tcl::BkdErrorHandler itsErrorHandler;
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
  itsWidget(),
  itsHostname(""),
  itsSubject(""),
  itsBeginDate(""),
  itsEndDate(""),
  itsAutosaveFile("__autosave_file"),
  itsInfoLog(),
  itsAutosavePeriod(10),
  itsBlocks(),
  itsCurrentBlockIdx(0),
  itsTimer(),
  itsErrorHandler(interp)
{
DOTRACE("ExptDriver::Impl::Impl");

  fstring cmd_line("command line: ");

  for (int i = 0; i < argc; ++i) {
    cmd_line.append(argv[i]);
    cmd_line.append(" ");
  }

  addLogInfo(cmd_line.c_str());
}

ExptDriver::Impl::~Impl()
{
DOTRACE("ExptDriver::Impl::~Impl");
}

void ExptDriver::Impl::doAutosave() {
DOTRACE("ExptDriver::Impl::doAutosave");
  try
    {
      DebugEvalNL(itsAutosaveFile.c_str());
      IO::saveASW(Util::Ref<IO::IoObject>(itsOwner), itsAutosaveFile);
    }
  catch (Util::Error& err)
    {
      itsErrorHandler.handleMsg(err.msg_cstr());
    }
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
  if ( !haveValidBlock() ) return false;

  return ( (itsAutosavePeriod > 0) &&
           ((currentBlock()->numCompleted() % itsAutosavePeriod) == 0) &&
           !(itsAutosaveFile.empty()) );
}

void ExptDriver::Impl::doUponCompletion() const {
DOTRACE("ExptDriver::Impl::doUponCompletion");
  if (itsInterp.hasCommand("Expt::doUponCompletion"))
    {
      Tcl::Code cmd("Expt::doUponCompletion", &itsErrorHandler);
      cmd.invoke(itsInterp);
    }
}

///////////////////////////////////////////////////////////////////////
//
// ExptDriver::Impl delegand functions
//
///////////////////////////////////////////////////////////////////////

void ExptDriver::Impl::readFrom(IO::Reader* reader) {
DOTRACE("ExptDriver::Impl::readFrom");

  reader->ensureReadVersionId("ExptDriver", 3, "Try grsh0.8a3");

  reader->readValue("hostname", itsHostname);
  reader->readValue("subject", itsSubject);
  reader->readValue("beginDate", itsBeginDate);
  reader->readValue("endDate", itsEndDate);
  reader->readValue("autosaveFile", itsAutosaveFile);
  reader->readValue("autosavePeriod", itsAutosavePeriod);
  reader->readValue("infoLog", itsInfoLog);

  reader->readValue("currentBlockIdx", itsCurrentBlockIdx);

  itsBlocks.clear();
  IO::ReadUtils::readObjectSeq<Block>(
           reader, "blocks", std::back_inserter(itsBlocks));

  fstring proc_body;
  reader->readValue("doUponCompletionScript", proc_body);
  itsInterp.createProc("Expt", "doUponCompletion", "", proc_body.c_str());
}

void ExptDriver::Impl::writeTo(IO::Writer* writer) const {
DOTRACE("ExptDriver::Impl::writeTo");

  writer->ensureWriteVersionId("ExptDriver", EXPTDRIVER_SERIAL_VERSION_ID, 3,
                               "Try grsh0.8a3");

  writer->writeValue("hostname", itsHostname);
  writer->writeValue("subject", itsSubject);
  writer->writeValue("beginDate", itsBeginDate);
  writer->writeValue("endDate", itsEndDate);
  writer->writeValue("autosaveFile", itsAutosaveFile);
  writer->writeValue("autosavePeriod", itsAutosavePeriod);
  writer->writeValue("infoLog", itsInfoLog);

  writer->writeValue("currentBlockIdx", itsCurrentBlockIdx);

  IO::WriteUtils::writeObjectSeq(writer, "blocks",
                                 itsBlocks.begin(), itsBlocks.end());

  fstring proc_body = itsInterp.getProcBody("Expt::doUponCompletion");
  writer->writeValue("doUponCompletionScript", proc_body);
}

///////////////////////////////////////////////////////////////////////
//
// ExptDriver action method definitions
//
///////////////////////////////////////////////////////////////////////


void ExptDriver::Impl::edBeginExpt() {
DOTRACE("ExptDriver::Impl::edBeginExpt");

  ensureHasBlock();

  addLogInfo("Beginning experiment.");

  itsBeginDate = System::theSystem().formattedTime();
  itsHostname = System::theSystem().getenv("HOST");
  itsSubject = System::theSystem().getcwd();

  itsTimer.restart();

  currentBlock()->beginTrial(*itsOwner);
}


void ExptDriver::Impl::edEndTrial() {
DOTRACE("ExptDriver::Impl::edEndTrial");

  if ( needAutosave() ) { doAutosave(); }
}


void ExptDriver::Impl::edNextBlock() {
DOTRACE("ExptDriver::Impl::edNextBlock");

  ++itsCurrentBlockIdx;

  if ( !haveValidBlock() )
    {
      edEndExpt();
    }
  else
    {
      currentBlock()->beginTrial(*itsOwner);
    }
}


void ExptDriver::Impl::edHaltExpt() const {
DOTRACE("ExptDriver::Impl::edHaltExpt");

  if ( haveValidBlock() )
    {
      currentBlock()->haltExpt();
    }
}


void ExptDriver::Impl::edResumeExpt() {
DOTRACE("ExptDriver::Impl::edResumeExpt");
  ensureHasBlock();

  currentBlock()->beginTrial(*itsOwner);
}


void ExptDriver::Impl::edClearExpt() {
DOTRACE("ExptDriver::Impl::edClearExpt");
  edHaltExpt();

  itsBlocks.clear();
  itsCurrentBlockIdx = 0;
}


void ExptDriver::Impl::edResetExpt() {
DOTRACE("ExptDriver::Impl::edResetExpt");
  edHaltExpt();

  while (itsCurrentBlockIdx > 0)
    {
      if ( haveValidBlock() )
        {
          currentBlock()->resetBlock();
        }

      --itsCurrentBlockIdx;
    }
}


void ExptDriver::Impl::edEndExpt() {
DOTRACE("ExptDriver::Impl::edEndExpt");

  Util::log() << "expt completed in "
              << itsTimer.elapsedMsec()
              << " milliseconds\n";

  addLogInfo("Experiment complete.");

  storeData();
  doUponCompletion();        // Call the user-defined callback
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

  edHaltExpt();

  try
    {
      itsEndDate = System::theSystem().formattedTime();

      fstring unique_file_extension =
        System::theSystem().formattedTime("%H%M%S%d%b%Y");

      // Write the main experiment file
      fstring expt_filename = "expt";
      expt_filename += unique_file_extension;
      expt_filename += ".asw";
      IO::saveASW(Util::Ref<IO::IoObject>(itsOwner), expt_filename.c_str());
      Util::log() << "wrote file " << expt_filename.c_str() << '\n';

      // Write the responses file
      fstring resp_filename = "resp";
      resp_filename += unique_file_extension;
      TlistUtils::writeResponses(resp_filename.c_str());
      Util::log() << "wrote file " << resp_filename.c_str() << '\n';

      // Change file access modes to allow read-only by all
      System::mode_t datafile_mode =
        System::IRUSR | System::IRGRP | System::IROTH;

      System::theSystem().chmod(expt_filename.c_str(), datafile_mode);
      System::theSystem().chmod(resp_filename.c_str(), datafile_mode);

      addLogInfo("Experiment saved.");
    }
  catch (Util::Error& err)
    {
      itsErrorHandler.handleMsg(err.msg_cstr());
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
  { return EXPTDRIVER_SERIAL_VERSION_ID; }

void ExptDriver::readFrom(IO::Reader* reader)
  { itsImpl->readFrom(reader); }

void ExptDriver::writeTo(IO::Writer* writer) const
  { itsImpl->writeTo(writer); }


///////////////////////////////////////////////////////////////////////
//
// ExptDriver accessor + manipulator method definitions
//
///////////////////////////////////////////////////////////////////////

const fstring& ExptDriver::getAutosaveFile() const
  { return itsImpl->itsAutosaveFile; }

void ExptDriver::setAutosaveFile(const fstring& str)
  { itsImpl->itsAutosaveFile = str; }

int ExptDriver::getAutosavePeriod() const
  { return itsImpl->itsAutosavePeriod; }

void ExptDriver::setAutosavePeriod(int period)
  { itsImpl->itsAutosavePeriod = period; }

const char* ExptDriver::getInfoLog() const
  { return itsImpl->itsInfoLog.c_str(); }

void ExptDriver::addLogInfo(const char* message)
  { itsImpl->addLogInfo(message); }

void ExptDriver::addBlock(Ref<Block> block)
  { itsImpl->addBlock(block); }

Ref<Block> ExptDriver::currentBlock() const
  { return itsImpl->currentBlock(); }

Util::ErrorHandler& ExptDriver::getErrorHandler() const
  { return itsImpl->itsErrorHandler; }

Util::WeakRef<GWT::Widget> ExptDriver::getWidget() const
  { return itsImpl->itsWidget; }

void ExptDriver::setWidget(Util::WeakRef<GWT::Widget> widg)
  { itsImpl->itsWidget = widg; }

GWT::Canvas& ExptDriver::getCanvas() const
  { return itsImpl->itsWidget->getCanvas(); }

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

void ExptDriver::edClearExpt()
  { itsImpl->edClearExpt(); }

void ExptDriver::edResetExpt()
  { itsImpl->edResetExpt(); }

void ExptDriver::storeData()
  { itsImpl->storeData(); }

static const char vcid_exptdriver_cc[] = "$Header$";
#endif // !EXPTDRIVER_CC_DEFINED
