///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 11 13:33:50 1999
// written: Wed Sep 25 19:01:23 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_H_DEFINED
#define EXPTDRIVER_H_DEFINED

#include "io/io.h"

#include "util/tracer.h"

#include "visx/experiment.h"

struct Tcl_Interp;

class Block;

class fstring;

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

class ExptDriver : public Experiment, public IO::IoObject
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

  void addBlock(Util::Ref<Block> block);
  Util::Ref<Block> currentBlock() const;

  /// Returns an iterator to all the Block's contained in the experiment.
  Util::FwdIter<Util::Ref<Block> > blocks() const;

  fstring getDoWhenComplete() const;
  void setDoWhenComplete(const fstring& script);

  virtual Util::ErrorHandler& getErrorHandler() const;

  Util::SoftRef<GWT::Widget> getWidget() const;
  void setWidget(Util::SoftRef<GWT::Widget> widg);

  virtual Gfx::Canvas& getCanvas() const;

  Tcl_Interp* getInterp() const;

  virtual void edBeginExpt();
  virtual void edEndTrial();
  virtual void edNextBlock();
  virtual void edHaltExpt() const;
  virtual void edResumeExpt();
  virtual void edClearExpt();
  virtual void edResetExpt();

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
