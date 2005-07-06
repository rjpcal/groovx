/** @file visx/exptdriver.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue May 11 13:33:50 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_EXPTDRIVER_CC_UTC20050626084017_DEFINED
#define GROOVX_VISX_EXPTDRIVER_CC_UTC20050626084017_DEFINED

#include "visx/exptdriver.h"

#include "gfx/toglet.h"

#include "io/ioproxy.h"
#include "io/ioutil.h"
#include "io/readutils.h"
#include "io/tclprocwrapper.h"
#include "io/writeutils.h"

#include "nub/log.h"
#include "nub/ref.h"

#include "tcl/eventloop.h"
#include "tcl/interp.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/iter.h"
#include "rutz/timeformat.h"
#include "rutz/unixcall.h"

#include "visx/tlistutils.h"

#include <cstdlib> // for getenv()
#include <cstring> // for strncmp()
#include <sys/stat.h> // for mode_t constants S_IRUSR etc.
#include <unistd.h> // for sleep()

#define GVX_DYNAMIC_TRACE_EXPR ExptDriver::tracer.status()
#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

rutz::tracer ExptDriver::tracer;

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId EXPTDRIVER_SVID = 6;
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
    interp(tcl::event_loop::interp()),
    widget(),
    hostname(""),
    subject(""),
    beginDate(""),
    endDate(""),
    autosaveFile("__autosave_file"),
    filePrefix("expt"),
    infoLog(),
    autosavePeriod(10),
    doWhenComplete(new tcl::ProcWrapper(interp)),
    numTrialsCompleted(0),
    createTime(rutz::time::wall_clock_now()),
    fileTimestamp(rutz::format_time(createTime, "%Y%b%d_%H%M%S"))
  {}

  ~Impl() {}

  void addLogInfo(const char* message)
  {
    const fstring date_string =
      rutz::format_time(rutz::time::wall_clock_now());

    infoLog.append("@");
    infoLog.append(date_string);
    infoLog.append(" ");
    infoLog.append(message);
    infoLog.append("\n");
  }

  //
  // data members
  //

  tcl::interpreter interp;

  nub::soft_ref<Toglet> widget;

  fstring hostname;     // Host computer on which Expt was begun
  fstring subject;      // Id of subject on whom Expt was performed
  fstring beginDate;    // Date(+time) when Expt was begun
  fstring endDate;      // Date(+time) when Expt was stopped
  fstring autosaveFile; // Filename used for autosaves
  fstring filePrefix;   // String used as filename prefix for output files

  fstring infoLog;

  int autosavePeriod;

  nub::ref<tcl::ProcWrapper> doWhenComplete;

  unsigned int numTrialsCompleted;

  rutz::time createTime;// Timestamp of when the ExptDriver is created
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
GVX_TRACE("ExptDriver::ExptDriver");

  rep->addLogInfo(tcl::event_loop::command_line().c_str());
}

ExptDriver::~ExptDriver() throw()
{
GVX_TRACE("ExptDriver::~ExptDriver");
  delete rep;
}

IO::VersionId ExptDriver::serialVersionId() const
{
GVX_TRACE("ExptDriver::serialVersionId");
  return EXPTDRIVER_SVID;
}

void ExptDriver::readFrom(IO::Reader& reader)
{
GVX_TRACE("ExptDriver::readFrom");

  reader.ensureReadVersionId("ExptDriver", 6,
                             "Try cvs tag xml_conversion_20040526",
                             SRC_POS);

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
GVX_TRACE("ExptDriver::writeTo");

  writer.ensureWriteVersionId("ExptDriver", EXPTDRIVER_SVID, 6,
                              "Try groovx0.8a7", SRC_POS);

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

const nub::soft_ref<Toglet>& ExptDriver::getWidget() const
{
GVX_TRACE("ExptDriver::getWidget");
  return rep->widget;
}

void ExptDriver::vxRun(Element& /*parent*/)
{
GVX_TRACE("currentElement");
  /* FIXME */ GVX_ASSERT(false);
}

void ExptDriver::vxEndTrialHook()
{
GVX_TRACE("ExptDriver::vxEndTrialHook");

  ++(rep->numTrialsCompleted);

  // Determine whether an autosave is necessary now. An autosave is
  // requested only if the autosave period is positive, and the number of
  // completed trials is evenly divisible by the autosave period.
  if ( rep->autosavePeriod <= 0 ||
       (rep->numTrialsCompleted % rep->autosavePeriod) != 0 ||
       rep->autosaveFile.is_empty() )
    return;

  dbg_eval_nl(3, rep->autosaveFile.c_str());
  IO::saveGVX(nub::ref<IO::IoObject>(this), rep->autosaveFile);
}

void ExptDriver::vxAllChildrenFinished()
{
GVX_TRACE("ExptDriver::vxAllChildrenFinished");

  nub::log( "ExptDriver::vxAllChildrenFinished" );

  rep->addLogInfo("Experiment complete.");

  vxHalt();

  storeData();

  nub::logging::add_obj_scope(*rep->doWhenComplete);
  rep->doWhenComplete->invoke(""); // Call the user-defined callback
  nub::logging::remove_obj_scope(*rep->doWhenComplete);

  nub::logging::remove_obj_scope(*this);
}

///////////////////////////////////////////////////////////////////////
//
// ExptDriver accessor + manipulator method definitions
//
///////////////////////////////////////////////////////////////////////

const fstring& ExptDriver::getAutosaveFile() const
{
GVX_TRACE("ExptDriver::getAutosaveFile");
  return rep->autosaveFile;
}

void ExptDriver::setAutosaveFile(const fstring& str)
{
GVX_TRACE("ExptDriver::setAutosaveFile");
  rep->autosaveFile = str;
}

const fstring& ExptDriver::getFilePrefix() const
{
GVX_TRACE("ExptDriver::getFilePrefix");
  return rep->filePrefix;
}

void ExptDriver::setFilePrefix(const fstring& str)
{
GVX_TRACE("ExptDriver::setFilePrefix");
  rep->filePrefix = str;
}

void ExptDriver::claimLogFile() const
{
GVX_TRACE("ExptDriver::claimLogFile");
  nub::logging::set_log_filename(fstring(rep->filePrefix, "_",
                                         rep->fileTimestamp, ".log"));
}

int ExptDriver::getAutosavePeriod() const
{
GVX_TRACE("ExptDriver::getAutosavePeriod");
  return rep->autosavePeriod;
}

void ExptDriver::setAutosavePeriod(int period)
{
GVX_TRACE("ExptDriver::setAutosavePeriod");
  rep->autosavePeriod = period;
}

const char* ExptDriver::getInfoLog() const
{
GVX_TRACE("ExptDriver::getInfoLog");
  return rep->infoLog.c_str();
}

fstring ExptDriver::getDoWhenComplete() const
{
GVX_TRACE("ExptDriver::getDoWhenComplete");
  return rep->doWhenComplete->fullSpec();
}

void ExptDriver::setDoWhenComplete(const fstring& script)
{
GVX_TRACE("ExptDriver::setDoWhenComplete");
  rep->doWhenComplete->define("", script);
}

void ExptDriver::setWidget(const nub::soft_ref<Toglet>& widg)
{
GVX_TRACE("ExptDriver::setWidget");
  rep->widget = widg;
}

void ExptDriver::edBeginExpt()
{
GVX_TRACE("ExptDriver::edBeginExpt");

  if (isComplete())
    {
      throw rutz::error("Can't begin experiment: "
                        "there are no pending elements", SRC_POS);
    }

  rep->addLogInfo("Beginning experiment.");

  const fstring cwd = rutz::unixcall::getcwd();

  rep->beginDate = rutz::format_time(rutz::time::wall_clock_now());
  rep->hostname = getenv("HOSTNAME");
  rep->subject = cwd;
  rep->numTrialsCompleted = 0;

  nub::logging::reset(); // to clear any existing timer scopes
  nub::logging::add_obj_scope(*this);

  claimLogFile();

  nub::log(fstring("expt begin: ", rep->beginDate));
  nub::log(fstring("hostname: ", rep->hostname));
  nub::log(fstring("cwd: ", cwd));
  nub::log(fstring("cmdline: ", tcl::event_loop::command_line()));

  currentElement()->vxRun(*this);
}

void ExptDriver::edResumeExpt()
{
GVX_TRACE("ExptDriver::edResumeExpt");

  if (isComplete())
    {
      throw rutz::error("Can't resume experiment: "
                        "there are no pending elements", SRC_POS);
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
GVX_TRACE("ExptDriver::pause");

  // Halt the experiment, then pop up a pause window. When the user
  // dismisses the window, the experiment will resume.

  fstring pauseMsgCmd
    ("tk_messageBox -default ok -icon info "
     "-title \"Pause\" -type ok "
     "-message \"Experiment paused. Click OK to continue.\";\n");

  vxHalt();

  rep->addLogInfo("Experiment paused.");

  rep->interp.eval(pauseMsgCmd);

  tcl::interpreter::clear_event_queue();

  rep->widget->fullClearscreen();
  rep->widget->fullClearscreen();

  ::sleep(2);

  rep->widget->fullClearscreen();
  rep->widget->fullClearscreen();

  tcl::interpreter::clear_event_queue();

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

    rutz::unixcall::rename(fname.c_str(), newname.c_str());
  }
}

void ExptDriver::storeData()
{
GVX_TRACE("ExptDriver::storeData");

  // The experiment and a summary of the responses to it are written to
  // files with unique filenames.

  const rutz::time timestamp = rutz::time::wall_clock_now();

  rep->endDate = rutz::format_time(timestamp);

  // Write the main experiment file
  fstring expt_filename = rep->filePrefix;
  expt_filename.append("_", rep->fileTimestamp);
  expt_filename.append(".gvx");
  renameFileIfExists(expt_filename);
  IO::saveGVX(nub::ref<IO::IoObject>(this), expt_filename.c_str());
  nub::log( fstring( "wrote file ", expt_filename.c_str()) );

  // Write the responses file
  fstring resp_filename = rep->filePrefix;
  resp_filename.append("_", rep->fileTimestamp);
  resp_filename.append(".resp");
  renameFileIfExists(resp_filename);
  TlistUtils::writeResponses(resp_filename.c_str());
  nub::log( fstring( "wrote file ", resp_filename.c_str()) );

  // Change file access modes to allow read-only by all
  const mode_t datafile_mode = S_IRUSR | S_IRGRP | S_IROTH;

  rutz::unixcall::chmod(expt_filename.c_str(), datafile_mode);
  rutz::unixcall::chmod(resp_filename.c_str(), datafile_mode);

  rep->addLogInfo("Experiment saved.");
}

static const char vcid_groovx_visx_exptdriver_cc_utc20050626084017[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_EXPTDRIVER_CC_UTC20050626084017_DEFINED
