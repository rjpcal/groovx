///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Thu Oct 26 16:55:04 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_H_DEFINED
#define EXPTDRIVER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(EXPERIMENT_H_DEFINED)
#include "experiment.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRACER_H_DEFINED)
#include "util/tracer.h"
#endif

struct Tcl_Interp;

class Block;
class ResponseHandler;
class TimingHdlr;

class fixed_string;

class ExptError : public ErrorWithMsg {
public:
  ExptError() : ErrorWithMsg() {}
  ExptError(const char* msg) : ErrorWithMsg(msg) {}
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

public:

  /** This tracer dynamically controls the tracing of ExptDriver
		member functions. */
  static Util::Tracer tracer;

  /// Construct with the applications Tcl interpreter.
  ExptDriver(int argc, char** argv, Tcl_Interp* interp);

  /// Virtual destructor.
  virtual ~ExptDriver();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  //////////////////////////////
  // Accessors + Manipulators //
  //////////////////////////////

  /// Return the name of the file currently being used for autosaves
  const fixed_string& getAutosaveFile() const;
  /// Change the name of the file to use for autosaves
  void setAutosaveFile(const fixed_string& str);

  /// Return the current autosave period.
  int getAutosavePeriod() const;
  /// Change the autosave period to \a period.
  void setAutosavePeriod(int period);

  /// Return the full contents of the info log.
  const char* getInfoLog() const;

  /** Add a message to the info log. The message will automatically be
		date/time-stamped. */
  void addLogInfo(const char* message);

  void addBlock(int block_id);
  int currentBlock() const;

  virtual Util::ErrorHandler& getErrorHandler();

  virtual GWT::Widget* getWidget();

  virtual GWT::Canvas* getCanvas();

  virtual void edBeginExpt();
  virtual void edEndTrial();
  virtual void edNextBlock();
  virtual void edHaltExpt() const;
  virtual void edResumeExpt();
  virtual void edClearExpt();
  virtual void edResetExpt();

  /// Returns the id of the current trial
  virtual void edSetCurrentTrial(int trial);

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
