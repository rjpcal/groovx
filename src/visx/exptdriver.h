///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 11 13:33:50 1999
// written: Wed Dec  4 19:08:46 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_H_DEFINED
#define EXPTDRIVER_H_DEFINED

#include "io/io.h"

#include "util/tracer.h"

#include "visx/element.h"

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

class ExptDriver : public Element
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

  virtual Util::ErrorHandler& getErrorHandler() const;

  virtual const Util::SoftRef<Toglet>& getWidget() const;

  virtual int trialType() const;

  virtual int numCompleted() const;

  virtual int lastResponse() const;

  /// Overridden from Element.
  virtual fstring status() const;

  virtual void vxRun(Element& parent);

  /// Halt the experiment. No more trials will be begun.
  virtual void vxHalt() const;

  virtual void vxAbort();

  /// End the current trial normally, and move on to the next trial.
  virtual void vxEndTrial();

  /** Attempt to start the next element, or stop the experiment if there
      are no more element. */
  virtual void vxNext();

  virtual void vxProcessResponse(Response& response);

  virtual void vxUndo();

  /** Reset the Experiment, restoring it to a state in which no
      trials have been run. */
  virtual void vxReset();

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

  void addElement(Util::Ref<Element> elem);
  Util::Ref<Element> currentElement() const;

  /// Returns an iterator to all the Element objects contained in the experiment.
  Util::FwdIter<Util::Ref<Element> > getElements() const;

  fstring getDoWhenComplete() const;
  void setDoWhenComplete(const fstring& script);

  void setWidget(const Util::SoftRef<Toglet>& widg);

  /// Return the Canvas for the Widget in which the experiment is running.
  Gfx::Canvas& getCanvas() const;

  Tcl_Interp* getInterp() const;

  /// Begin the experiment.
  void edBeginExpt();

  /// Resume the experiment after it has been halted.
  void edResumeExpt();

  /// Clear the experiment to an empty state.
  void edClearExpt();

  void pause();

  /** This saves the experiment file and a summary-of-responses file
      under unique filenames based on the date and time. */
  void storeData();

private:

  class Impl;
  friend class Impl;

  Impl* const itsImpl;
};

static const char vcid_exptdriver_h[] = "$Header$";
#endif // !EXPTDRIVER_H_DEFINED
