///////////////////////////////////////////////////////////////////////
//
// exptdriver.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue May 11 13:33:50 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_CC_DEFINED
#define EXPTDRIVER_CC_DEFINED

#include "visx/exptdriver.h"

#include "gfx/toglet.h"

#include "io/ioproxy.h"
#include "io/ioutil.h"
#include "io/readutils.h"
#include "io/tclprocwrapper.h"
#include "io/writeutils.h"

#include "system/system.h"

#include "tcl/tclmain.h"
#include "tcl/tclsafeinterp.h"

#include "util/error.h"
#include "util/iter.h"
#include "util/log.h"
#include "util/ref.h"
#include "util/strings.h"

#include "visx/tlistutils.h"

#include <cstdlib> // for getenv()
#include <cstring> // for strncmp()
#include <sys/stat.h> // for mode_t constants S_IRUSR etc.
#include <unistd.h> // for sleep()

#define DYNAMIC_TRACE_EXPR ExptDriver::tracer.status()
#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

Util::Tracer ExptDriver::tracer;

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId EXPTDRIVER_SERIAL_VERSION_ID = 6;
}

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
    interp(Tcl::Main::interp()),
    widget(),
    hostname(""),
    subject(""),
    beginDate(""),
    endDate(""),
    autosaveFile("__autosave_file"),
    filePrefix("expt"),
    infoLog(),
    autosavePeriod(10),
    doWhenComplete(new Tcl::ProcWrapper(interp)),
    numTrialsCompleted(0),
    createTime(Util::Time::wallClockNow()),
    fileTimestamp(createTime.format("%Y%b%d_%H%M%S"))
  {}

  ~Impl() {}

  void addLogInfo(const char* message)
  {
    const fstring date_string = Util::Time::wallClockNow().format();

    infoLog.append("@");
    infoLog.append(date_string);
    infoLog.append(" ");
    infoLog.append(message);
    infoLog.append("\n");
  }

  //
  // data members
  //

  Tcl::Interp interp;

  Util::SoftRef<Toglet> widget;

  fstring hostname;     // Host computer on which Expt was begun
  fstring subject;      // Id of subject on whom Expt was performed
  fstring beginDate;    // Date(+time) when Expt was begun
  fstring endDate;      // Date(+time) when Expt was stopped
  fstring autosaveFile; // Filename used for autosaves
  fstring filePrefix;   // String used as filename prefix for output files

  fstring infoLog;

  int autosavePeriod;

  Util::Ref<Tcl::ProcWrapper> doWhenComplete;

  unsigned int numTrialsCompleted;

  Util::Time createTime;// Timestamp of when the ExptDriver is created
  fstring fileTimestamp;// Timestamp used in filenames
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

  rep->addLogInfo(Tcl::Main::commandLine().c_str());
}

ExptDriver::~ExptDriver() throw()
{
DOTRACE("ExptDriver::~ExptDriver");
  delete rep;
}

IO::VersionId ExptDriver::serialVersionId() const
{
DOTRACE("ExptDriver::serialVersionId");
  return EXPTDRIVER_SERIAL_VERSION_ID;
}

void ExptDriver::readFrom(IO::Reader& reader)
{
DOTRACE("ExptDriver::readFrom");

  reader.ensureReadVersionId("ExptDriver", 6,
                             "Try cvs tag xml_conversion_20040526");

  reader.readValue("hostname", rep->hostname);
  reader.readValue("subject", rep->subject);
  reader.readValue("beginDate", rep->beginDate);
  reader.readValue("endDate", rep->endDate);
  reader.readValue("autosaveFile", rep->autosaveFile);
  reader.readValue("autosavePeriod", rep->autosavePeriod);
  reader.readValue("infoLog", rep->infoLog);
  reader.readOwnedObject("doWhenComplete", rep->doWhenComplete);
  reader.readValue("filePrefix", rep->filePrefix);

  reader.readBaseClass("ElementContainer",
                       IO::makeProxy<ElementContainer>(this));
}

void ExptDriver::writeTo(IO::Writer& writer) const
{
DOTRACE("ExptDriver::writeTo");

  writer.ensureWriteVersionId("ExptDriver", EXPTDRIVER_SERIAL_VERSION_ID, 6,
                              "Try groovx0.8a7");

  writer.writeValue("hostname", rep->hostname);
  writer.writeValue("subject", rep->subject);
  writer.writeValue("beginDate", rep->beginDate);
  writer.writeValue("endDate", rep->endDate);
  writer.writeValue("autosaveFile", rep->autosaveFile);
  writer.writeValue("autosavePeriod", rep->autosavePeriod);
  writer.writeValue("infoLog", rep->infoLog);
  writer.writeOwnedObject("doWhenComplete", rep->doWhenComplete);
  writer.writeValue("filePrefix", rep->filePrefix);

  writer.writeBaseClass("ElementContainer",
                        IO::makeConstProxy<ElementContainer>(this));
}


///////////////////////////////////////////////////////////////////////
//
// ExptDriver's Element interface
//
///////////////////////////////////////////////////////////////////////

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
  IO::saveGVX(Util::Ref<IO::IoObject>(this), rep->autosaveFile);
}

void ExptDriver::vxAllChildrenFinished()
{
DOTRACE("ExptDriver::vxAllChildrenFinished");

  Util::log( "ExptDriver::vxAllChildrenFinished" );

  rep->addLogInfo("Experiment complete.");

  vxHalt();

  storeData();

  Util::Log::addObjScope(*rep->doWhenComplete);
  rep->doWhenComplete->invoke(""); // Call the user-defined callback
  Util::Log::removeObjScope(*rep->doWhenComplete);

  Util::Log::removeObjScope(*this);
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

const fstring& ExptDriver::getFilePrefix() const
{
DOTRACE("ExptDriver::getFilePrefix");
  return rep->filePrefix;
}

void ExptDriver::setFilePrefix(const fstring& str)
{
DOTRACE("ExptDriver::setFilePrefix");
  rep->filePrefix = str;
}

void ExptDriver::claimLogFile() const
{
DOTRACE("ExptDriver::claimLogFile");
  Util::Log::setLogFilename(fstring(rep->filePrefix, "_",
                                    rep->fileTimestamp, ".log"));
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

  if (isComplete())
    {
      throw Util::Error("Can't begin experiment: there are no pending elements");
    }

  rep->addLogInfo("Beginning experiment.");

  const fstring cwd = unixcall::getcwd();

  rep->beginDate = Util::Time::wallClockNow().format();
  rep->hostname = getenv("HOSTNAME");
  rep->subject = cwd;
  rep->numTrialsCompleted = 0;

  Util::Log::reset(); // to clear any existing timer scopes
  Util::Log::addObjScope(*this);

  claimLogFile();

  Util::log(fstring("expt begin: ", rep->beginDate));
  Util::log(fstring("hostname: ", rep->hostname));
  Util::log(fstring("cwd: ", cwd));
  Util::log(fstring("cmdline: ", Tcl::Main::commandLine()));

  currentElement()->vxRun(*this);
}

void ExptDriver::edResumeExpt()
{
DOTRACE("ExptDriver::edResumeExpt");

  if (isComplete())
    {
      throw Util::Error("Can't resume experiment: there are no pending elements");
    }

  currentElement()->vxRun(*this);
}

#if 0
// This is disabled right now because it interacts badly with top-level
// Toglet windows that have override_direct=true... neither the Toglet nor
// the tk_messageBox are unable to receive any events, so the application
// appears to lock up
void ExptDriver::pause()
{
DOTRACE("ExptDriver::pause");

  // Halt the experiment, then pop up a pause window. When the user
  // dismisses the window, the experiment will resume.

  fstring pauseMsgCmd
    ("tk_messageBox -default ok -icon info "
     "-title \"Pause\" -type ok "
     "-message \"Experiment paused. Click OK to continue.\";\n");

  vxHalt();

  rep->addLogInfo("Experiment paused.");

  rep->interp.eval(pauseMsgCmd);

  Tcl::Interp::clearEventQueue();

  rep->widget->fullClearscreen();
  rep->widget->fullClearscreen();

  ::sleep(2);

  rep->widget->fullClearscreen();
  rep->widget->fullClearscreen();

  Tcl::Interp::clearEventQueue();

  rep->addLogInfo("Resuming experiment.");

  edResumeExpt();
}
#endif

namespace
{
  // Check if fname exists as a file; if so, then rename it to a
  // unique name so that at the end of this call, no file exists with
  // the given fname.
  void renameFileIfExists(const fstring& fname)
  {
    struct stat buf;

    if (stat(fname.c_str(), &buf) != 0)
      {
        // no file exists at fname, so we don't need to do anything
        return;
      }

    fstring newname = fname;

    int i = 0;

    while (stat(newname.c_str(), &buf) == 0)
      {
        newname = fname;
        newname.append(".bkp", i++);
      }

    unixcall::rename(fname.c_str(), newname.c_str());
  }
}

void ExptDriver::storeData()
{
DOTRACE("ExptDriver::storeData");

  // The experiment and a summary of the responses to it are written to
  // files with unique filenames.

  const Util::Time timestamp = Util::Time::wallClockNow();

  rep->endDate = timestamp.format();

  // Write the main experiment file
  fstring expt_filename = rep->filePrefix;
  expt_filename.append("_", rep->fileTimestamp);
  expt_filename.append(".gvx");
  renameFileIfExists(expt_filename);
  IO::saveGVX(Util::Ref<IO::IoObject>(this), expt_filename.c_str());
  Util::log( fstring( "wrote file ", expt_filename.c_str()) );

  // Write the responses file
  fstring resp_filename = rep->filePrefix;
  resp_filename.append("_", rep->fileTimestamp);
  resp_filename.append(".resp");
  renameFileIfExists(resp_filename);
  TlistUtils::writeResponses(resp_filename.c_str());
  Util::log( fstring( "wrote file ", resp_filename.c_str()) );

  // Change file access modes to allow read-only by all
  const mode_t datafile_mode = S_IRUSR | S_IRGRP | S_IROTH;

  unixcall::chmod(expt_filename.c_str(), datafile_mode);
  unixcall::chmod(resp_filename.c_str(), datafile_mode);

  rep->addLogInfo("Experiment saved.");
}

static const char vcid_exptdriver_cc[] = "$Header$";
#endif // !EXPTDRIVER_CC_DEFINED
