///////////////////////////////////////////////////////////////////////
//
// exptdriver.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 11 13:33:50 1999
// written: Thu Dec  5 17:53:08 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_CC_DEFINED
#define EXPTDRIVER_CC_DEFINED

#include "visx/exptdriver.h"

#include "grsh/grsh.h"

#include "io/ioutil.h"
#include "io/readutils.h"
#include "io/writeutils.h"

#include "system/system.h"

#include "tcl/tclcode.h"
#include "tcl/tclerror.h"
#include "tcl/tclmain.h"
#include "tcl/tclprocwrapper.h"
#include "tcl/tclsafeinterp.h"
#include "tcl/toglet.h"

#include "util/error.h"
#include "util/errorhandler.h"
#include "util/iter.h"
#include "util/log.h"
#include "util/minivec.h"
#include "util/ref.h"
#include "util/strings.h"

#include "visx/tlistutils.h"

#define DYNAMIC_TRACE_EXPR ExptDriver::tracer.status()
#include "util/trace.h"
#include "util/debug.h"

Util::Tracer ExptDriver::tracer;

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId EXPTDRIVER_SERIAL_VERSION_ID = 4;
};

///////////////////////////////////////////////////////////////////////
//
// ExptDriver::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class ExptDriver::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl() :
    interp(Tcl::Main::safeInterp()),
    widget(),
    hostname(""),
    subject(""),
    beginDate(""),
    endDate(""),
    autosaveFile("__autosave_file"),
    infoLog(),
    autosavePeriod(10),
    errorHandler(interp.intp()),
    doWhenComplete(new Tcl::ProcWrapper(interp)),
    numTrialsCompleted(0)
  {}

  ~Impl() {}

  //
  // data members
  //

  Tcl::Interp interp;

  SoftRef<Toglet> widget;

  fstring hostname;     // Host computer on which Expt was begun
  fstring subject;      // Id of subject on whom Expt was performed
  fstring beginDate;    // Date(+time) when Expt was begun
  fstring endDate;      // Date(+time) when Expt was stopped
  fstring autosaveFile; // Filename used for autosaves

  fstring infoLog;

  int autosavePeriod;

  Tcl::BkdErrorHandler errorHandler;

  Ref<Tcl::ProcWrapper> doWhenComplete;

  unsigned int numTrialsCompleted;
};

///////////////////////////////////////////////////////////////////////
//
// ExptDriver creator method definitions
//
///////////////////////////////////////////////////////////////////////

ExptDriver::ExptDriver() :
  rep(new Impl)
{
DOTRACE("ExptDriver::ExptDriver");

  int argc = Grsh::argc();
  char** argv = Grsh::argv();

  fstring cmd_line("command line: ");

  for (int i = 0; i < argc; ++i)
  {
    cmd_line.append(argv[i]);
    cmd_line.append(" ");
  }

  addLogInfo(cmd_line.c_str());
}

ExptDriver::~ExptDriver()
{
DOTRACE("ExptDriver::~ExptDriver");
  delete rep;
}

IO::VersionId ExptDriver::serialVersionId() const
{
DOTRACE("ExptDriver::serialVersionId");
  return EXPTDRIVER_SERIAL_VERSION_ID;
}

void ExptDriver::readFrom(IO::Reader* reader)
{
DOTRACE("ExptDriver::readFrom");

  int svid = reader->ensureReadVersionId("ExptDriver", 3, "Try grsh0.8a3");

  reader->readValue("hostname", rep->hostname);
  reader->readValue("subject", rep->subject);
  reader->readValue("beginDate", rep->beginDate);
  reader->readValue("endDate", rep->endDate);
  reader->readValue("autosaveFile", rep->autosaveFile);
  reader->readValue("autosavePeriod", rep->autosavePeriod);
  reader->readValue("infoLog", rep->infoLog);

  reader->readValue("currentBlockIdx", iolegacySequencePos());

  iolegacyElements().clear();
  IO::ReadUtils::readObjectSeq<Element>(
           reader, "blocks", std::back_inserter(iolegacyElements()));

  if (svid < 4)
    {
      fstring proc_body;
      reader->readValue("doUponCompletionScript", proc_body);
      rep->doWhenComplete->define("", proc_body);
    }
  else
    {
      reader->readOwnedObject("doWhenComplete", rep->doWhenComplete);
    }
}

void ExptDriver::writeTo(IO::Writer* writer) const
{
DOTRACE("ExptDriver::writeTo");

  writer->ensureWriteVersionId("ExptDriver", EXPTDRIVER_SERIAL_VERSION_ID, 4,
                               "Try grsh0.8a7");

  writer->writeValue("hostname", rep->hostname);
  writer->writeValue("subject", rep->subject);
  writer->writeValue("beginDate", rep->beginDate);
  writer->writeValue("endDate", rep->endDate);
  writer->writeValue("autosaveFile", rep->autosaveFile);
  writer->writeValue("autosavePeriod", rep->autosavePeriod);
  writer->writeValue("infoLog", rep->infoLog);

  writer->writeValue("currentBlockIdx", iolegacySequencePos());

  IO::WriteUtils::writeObjectSeq(writer, "blocks",
                                 iolegacyElements().begin(),
                                 iolegacyElements().end());

  writer->writeOwnedObject("doWhenComplete", rep->doWhenComplete);
}


///////////////////////////////////////////////////////////////////////
//
// ExptDriver's Element interface
//
///////////////////////////////////////////////////////////////////////

Util::ErrorHandler& ExptDriver::getErrorHandler() const
{
DOTRACE("ExptDriver::getErrorHandler");
  return rep->errorHandler;
}

const Util::SoftRef<Toglet>& ExptDriver::getWidget() const
{
DOTRACE("ExptDriver::getWidget");
  return rep->widget;
}

void ExptDriver::vxRun(Element& /*parent*/)
{
DOTRACE("currentElement");
  /* FIXME */ Assert(false);
}

void ExptDriver::vxEndTrialHook()
{
DOTRACE("ExptDriver::vxEndTrialHook");

  ++(rep->numTrialsCompleted);

  // Determine whether an autosave is necessary now. An autosave is
  // requested only if the autosave period is positive, and the number of
  // completed trials is evenly divisible by the autosave period.
  if ( rep->autosavePeriod <= 0 ||
       (rep->numTrialsCompleted % rep->autosavePeriod) != 0 ||
       rep->autosaveFile.is_empty() )
    return;

  dbgEvalNL(3, rep->autosaveFile.c_str());
  IO::saveASW(Util::Ref<IO::IoObject>(this), rep->autosaveFile);
}

void ExptDriver::vxChildFinished(ChildStatus s)
{
DOTRACE("ExptDriver::vxChildFinished");

  childFinishedHelper(s);

  if ( isComplete() )
    {
      Util::log( "experiment complete" );

      addLogInfo("Experiment complete.");

      storeData();

      Util::log( fstring("Expt::doWhenComplete") );
      rep->doWhenComplete->invoke(""); // Call the user-defined callback

      Util::Log::removeScope("Expt");
    }
  else
    {
      currentElement()->vxRun(*this);
    }
}

///////////////////////////////////////////////////////////////////////
//
// ExptDriver accessor + manipulator method definitions
//
///////////////////////////////////////////////////////////////////////

const fstring& ExptDriver::getAutosaveFile() const
{
DOTRACE("ExptDriver::getAutosaveFile");
  return rep->autosaveFile;
}

void ExptDriver::setAutosaveFile(const fstring& str)
{
DOTRACE("ExptDriver::setAutosaveFile");
  rep->autosaveFile = str;
}

int ExptDriver::getAutosavePeriod() const
{
DOTRACE("ExptDriver::getAutosavePeriod");
  return rep->autosavePeriod;
}

void ExptDriver::setAutosavePeriod(int period)
{
DOTRACE("ExptDriver::setAutosavePeriod");
  rep->autosavePeriod = period;
}

const char* ExptDriver::getInfoLog() const
{
DOTRACE("ExptDriver::getInfoLog");
  return rep->infoLog.c_str();
}

void ExptDriver::addLogInfo(const char* message)
{
DOTRACE("ExptDriver::addLogInfo");
  fstring date_string = System::theSystem().formattedTime();

  rep->infoLog.append("@");
  rep->infoLog.append(date_string);
  rep->infoLog.append(" ");
  rep->infoLog.append(message);
  rep->infoLog.append("\n");
}

fstring ExptDriver::getDoWhenComplete() const
{
DOTRACE("ExptDriver::getDoWhenComplete");
  return rep->doWhenComplete->fullSpec();
}

void ExptDriver::setDoWhenComplete(const fstring& script)
{
DOTRACE("ExptDriver::setDoWhenComplete");
  rep->doWhenComplete->define("", script);
}

void ExptDriver::setWidget(const Util::SoftRef<Toglet>& widg)
{
DOTRACE("ExptDriver::setWidget");
  rep->widget = widg;
}

Gfx::Canvas& ExptDriver::getCanvas() const
{
DOTRACE("ExptDriver::getCanvas");
  return rep->widget->getCanvas();
}

void ExptDriver::edBeginExpt()
{
DOTRACE("ExptDriver::edBeginExpt");

  ensureNotComplete();

  addLogInfo("Beginning experiment.");

  rep->beginDate = System::theSystem().formattedTime();
  rep->hostname = System::theSystem().getenv("HOST");
  rep->subject = System::theSystem().getcwd();
  rep->numTrialsCompleted = 0;

  Util::Log::reset(); // to clear any existing timer scopes
  Util::Log::addScope("Expt");

  currentElement()->vxRun(*this);
}

void ExptDriver::edResumeExpt()
{
DOTRACE("ExptDriver::edResumeExpt");

  ensureNotComplete();

  currentElement()->vxRun(*this);
}

void ExptDriver::pause()
{
DOTRACE("ExptDriver::pause");

  // Halt the experiment, then pop up a pause window. When the user
  // dismisses the window, the experiment will resume.

  Tcl::Code pauseMsgCmd
  (Tcl::toTcl("tk_messageBox -default ok -icon info "
              "-title \"Pause\" -type ok "
              "-message \"Experiment paused. Click OK to continue.\";\n"),
   Tcl::Code::THROW_EXCEPTION);

  vxHalt();

  addLogInfo("Experiment paused.");

  pauseMsgCmd.invoke(rep->interp);

  Tcl::Interp::clearEventQueue();

  rep->widget->fullClearscreen();
  rep->widget->fullClearscreen();

  System::theSystem().sleep(2);

  rep->widget->fullClearscreen();
  rep->widget->fullClearscreen();

  Tcl::Interp::clearEventQueue();

  addLogInfo("Resuming experiment.");

  edResumeExpt();
}

void ExptDriver::storeData()
{
DOTRACE("ExptDriver::storeData");

  vxHalt();

  // The experiment and a summary of the responses to it are written to
  // files with unique filenames.

  rep->endDate = System::theSystem().formattedTime();

  fstring unique_file_extension =
    System::theSystem().formattedTime("%H%M%S%d%b%Y");

  // Write the main experiment file
  fstring expt_filename = "expt";
  expt_filename.append(unique_file_extension);
  expt_filename.append(".asw");
  IO::saveASW(Util::Ref<IO::IoObject>(this), expt_filename.c_str());
  Util::log( fstring( "wrote file ", expt_filename.c_str()) );

  // Write the responses file
  fstring resp_filename = "resp";
  resp_filename.append(unique_file_extension);
  TlistUtils::writeResponses(resp_filename.c_str());
  Util::log( fstring( "wrote file ", resp_filename.c_str()) );

  // Change file access modes to allow read-only by all
  System::mode_t datafile_mode =
    System::IRUSR | System::IRGRP | System::IROTH;

  System::theSystem().chmod(expt_filename.c_str(), datafile_mode);
  System::theSystem().chmod(resp_filename.c_str(), datafile_mode);

  addLogInfo("Experiment saved.");
}

static const char vcid_exptdriver_cc[] = "$Header$";
#endif // !EXPTDRIVER_CC_DEFINED
