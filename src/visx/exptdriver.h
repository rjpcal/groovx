/** @file visx/exptdriver.h */

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

#ifndef GROOVX_VISX_EXPTDRIVER_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_EXPTDRIVER_H_UTC20050626084016_DEFINED

#include "io/io.h"

#include "rutz/tracer.h"

#include "visx/elementcontainer.h"

struct Tcl_Interp;

namespace rutz
{
  class fstring;
  template <class T> class fwd_iter;
}

namespace Gfx
{
  class Canvas;
}

namespace nub
{
  template <class T> class ref;
  template <class T> class soft_ref;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * ExptDriver is an implementation of the Experiment interface that
 * coordinates all objects necessary to run an experiment.
 *
 **/
///////////////////////////////////////////////////////////////////////

class ExptDriver : public ElementContainer
{
private:
  /// Copy constructor not allowed
  ExptDriver(const ExptDriver&);
  /// assignment not allowed
  ExptDriver& operator=(const ExptDriver&);

  /// Construct with the applications Tcl interpreter.
  ExptDriver();

public:

  /// Dynamically controls the tracing of ExptDriver member functions.
  static rutz::tracer tracer;

  /// Factory function.
  static ExptDriver* make() { return new ExptDriver; }

  /// Virtual destructor.
  virtual ~ExptDriver() throw();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  //
  // Element interface
  //

  virtual const nub::soft_ref<Toglet>& getWidget() const;

  virtual void vxRun(Element& parent);

  /// End the current trial normally, and move on to the next trial.
  virtual void vxEndTrialHook();

  /// Stop the experiment since all child elements are finished.
  virtual void vxAllChildrenFinished();

  //////////////////////////////
  // Accessors + Manipulators //
  //////////////////////////////

  /// Return the name of the file currently being used for autosaves
  const rutz::fstring& getAutosaveFile() const;
  /// Change the name of the file to use for autosaves
  void setAutosaveFile(const rutz::fstring& str);

  /// Return the current autosave period.
  int getAutosavePeriod() const;

  /// Change the autosave period to \a period.
  void setAutosavePeriod(int period);

  /// Get the string used as a prefix for output files generated by the experiment.
  const rutz::fstring& getFilePrefix() const;

  /// Specify a string to be used as a prefix for output files generated by the experiment.
  void setFilePrefix(const rutz::fstring& str);

  /// Make the global log file (handled by nub::logging) be named after this expt.
  void claimLogFile() const;

  /// Return the full contents of the info log.
  const char* getInfoLog() const;

  /// Get the script to be executed when the experiment completes.
  rutz::fstring getDoWhenComplete() const;

  /// Specify a script to be executed when the experiment completes.
  void setDoWhenComplete(const rutz::fstring& script);

  /// Specify the widget in which the experiment should run.
  void setWidget(const nub::soft_ref<Toglet>& widg);

  /// Begin the experiment.
  void edBeginExpt();

  /// Resume the experiment after it has been halted.
  void edResumeExpt();

#if 0
  /// Pause the experiment until a top-level dialog box is dismissed.
  void pause();
#endif

  /** This saves the experiment file and a summary-of-responses file
      under unique filenames based on the date and time. */
  void storeData();

private:
  class Impl;

  Impl* const rep;
};

static const char vcid_groovx_visx_exptdriver_h_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_EXPTDRIVER_H_UTC20050626084016_DEFINED
