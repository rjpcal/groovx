///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 11 13:33:50 1999
// written: Wed Aug  8 20:16:38 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_H_DEFINED
#define EXPTDRIVER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(EXPERIMENT_H_DEFINED)
#include "experiment.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRACER_H_DEFINED)
#include "util/tracer.h"
#endif

struct Tcl_Interp;

class Block;

class fstring;

namespace Util
{
  template <class T> class Ref;
  template <class T> class WeakRef;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * ExptDriver is an implementation of the Experiment interface that
 * coordinates all objects necessary to run an experiment.
 *
 **/
///////////////////////////////////////////////////////////////////////

class ExptDriver : public Experiment, public virtual IO::IoObject {
private:
  /// Copy constructor not allowed
  ExptDriver(const ExptDriver&);
  /// assignment not allowed
  ExptDriver& operator=(const ExptDriver&);

  /// Construct with the applications Tcl interpreter.
  ExptDriver(int argc, char** argv, Tcl_Interp* interp);

public:

  /** This tracer dynamically controls the tracing of ExptDriver
      member functions. */
  static Util::Tracer tracer;

  static ExptDriver* make(int argc, char** argv, Tcl_Interp* interp)
    { return new ExptDriver(argc, argv, interp); }

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

  virtual Util::ErrorHandler& getErrorHandler() const;

  Util::WeakRef<GWT::Widget> getWidget() const;
  void setWidget(Util::WeakRef<GWT::Widget> widg);

  virtual GWT::Canvas& getCanvas() const;

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
