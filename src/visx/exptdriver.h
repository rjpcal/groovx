///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 11 13:33:50 1999
// written: Mon Jan 20 12:43:34 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_H_DEFINED
#define EXPTDRIVER_H_DEFINED

#include "io/io.h"

#include "util/tracer.h"

#include "visx/elementcontainer.h"

struct Tcl_Interp;

class fstring;

namespace Gfx
{
  class Canvas;
}

namespace Util
{
  template <class T> class FwdIter;
  template <class T> class Ref;
  template <class T> class SoftRef;
};

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

  /** This tracer dynamically controls the tracing of ExptDriver
      member functions. */
  static Util::Tracer tracer;

  static ExptDriver* make() { return new ExptDriver; }

  /// Virtual destructor.
  virtual ~ExptDriver();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  //
  // Element interface
  //

  virtual const Util::SoftRef<Toglet>& getWidget() const;

  virtual void vxRun(Element& parent);

  /// End the current trial normally, and move on to the next trial.
  virtual void vxEndTrialHook();

  /// Stop the experiment since all child elements are finished.
  virtual void vxAllChildrenFinished();

  //////////////////////////////
  // Accessors + Manipulators //
  //////////////////////////////

  /// Return the name of the file currently being used for autosaves
  const fstring& getAutosaveFile() const;
  /// Change the name of the file to use for autosaves
  void setAutosaveFile(const fstring& str);

  /// Return the current autosave period.
  int getAutosavePeriod() const;
  /// Change the autosave period to \a period.
  void setAutosavePeriod(int period);

  /// Return the full contents of the info log.
  const char* getInfoLog() const;

  /** Add a message to the info log. The message will automatically be
      date/time-stamped. */
  void addLogInfo(const char* message);

  fstring getDoWhenComplete() const;
  void setDoWhenComplete(const fstring& script);

  void setWidget(const Util::SoftRef<Toglet>& widg);

  /// Return the Canvas for the Widget in which the experiment is running.
  Gfx::Canvas& getCanvas() const;

  /// Begin the experiment.
  void edBeginExpt();

  /// Resume the experiment after it has been halted.
  void edResumeExpt();

  void pause();

  /** This saves the experiment file and a summary-of-responses file
      under unique filenames based on the date and time. */
  void storeData();

private:
  class Impl;
  friend class Impl;

  Impl* const rep;
};

static const char vcid_exptdriver_h[] = "$Header$";
#endif // !EXPTDRIVER_H_DEFINED
