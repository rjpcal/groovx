///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Wed Feb 16 16:01:35 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPTDRIVER_H_DEFINED
#define EXPTDRIVER_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

#ifndef EXPERIMENT_H_DEFINED
#include "experiment.h"
#endif

struct Tcl_Interp;

class Block;
class ResponseHandler;
class TimingHdlr;

class ExptError : public ErrorWithMsg {
public:
  ExptError(const string& msg="") : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * ExptDriver is an implementation of the Experiment interface that
 * coordinates all objects necessary to run an experiment.
 *
 **/
///////////////////////////////////////////////////////////////////////

class ExptDriver : public Experiment, public virtual IO {
private:
  /// Copy constructor not allowed
  ExptDriver(const ExptDriver&);
  /// assignment not allowed
  ExptDriver& operator=(const ExptDriver&);

public:
  /// Construct with the applications Tcl interpreter.
  ExptDriver(Tcl_Interp* interp);

  /// Virtual destructor.
  virtual ~ExptDriver();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  virtual void manageObject(const char* name, IO* object);

  //////////////////////////////
  // Accessors + Manipulators //
  //////////////////////////////

  /// Return the Tcl interpreter that was passed to the constructor.
  Tcl_Interp* getInterp();

  /// Return the name of the file currently being used for autosaves
  const string& getAutosaveFile() const;
  /// Change the name of the file to use for autosaves
  void setAutosaveFile(const string& str);

  virtual Widget* getWidget();

  virtual Canvas* getCanvas();

  virtual void edDraw();
  virtual void edUndraw();
  virtual void edSwapBuffers();

  virtual void edBeginExpt();

  virtual void edBeginTrial();
  virtual void edResponseSeen();
  virtual void edProcessResponse(int response);
  virtual void edAbortTrial();
  virtual void edEndTrial();
  virtual void edHaltExpt() const;

  virtual void edResetExpt();

  /// Returns the id of the current trial
  virtual int edGetCurrentTrial() const;
  virtual void edSetCurrentTrial(int trial);

  /// Uses \c deserialize() to read an experiment from \a filename.
  void read(const char* filename);

  /// Uses \c serialize() to write an experiment to \a filename.
  void write(const char* filename) const;

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
