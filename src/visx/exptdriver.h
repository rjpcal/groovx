///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Wed Dec  1 15:40:21 1999
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
 * ExptDriver is a singleton that coordinates all objects necessary to
 * run an experiment.
 *
 * @memo ExptDriver is a singleton that coordinates all objects
 * necessary to run an experiment.
 **/
///////////////////////////////////////////////////////////////////////

class ExptDriver : public Experiment, public virtual IO {
private:
  /// Copy constructor not allowed
  ExptDriver(const ExptDriver&);
  /// assignment not allowed
  ExptDriver& operator=(const ExptDriver&);

public:
  /// Default constructor
  ExptDriver();

  ///
  virtual ~ExptDriver();

  ///
  virtual void serialize(ostream &os, IOFlag flag) const;
  ///
  virtual void deserialize(istream &is, IOFlag flag);
  ///
  virtual int charCount() const;

  ///
  virtual void readFrom(Reader* reader);
  ///
  virtual void writeTo(Writer* writer) const;

  //////////////////////////////
  // Accessors + Manipulators //
  //////////////////////////////

  ///
  Tcl_Interp* getInterp();
  ///
  void setInterp(Tcl_Interp* interp);

  ///
  const string& getAutosaveFile() const;
  ///
  void setAutosaveFile(const string& str);


  /** @name Graphics Actions */
  //@{
  ///
  virtual void draw();
  ///
  virtual void undraw();
  ///
  virtual void edSwapBuffers();
  //@}

  /** @name Trial event sequence actions */
  //@{
  ///
  virtual void edBeginExpt();

  ///
  virtual void edBeginTrial();
  ///
  virtual void edResponseSeen();
  ///
  virtual void edProcessResponse(int response);
  ///
  virtual void edAbortTrial();
  ///
  virtual void edEndTrial();
  ///
  virtual void edHaltExpt() const;

  ///
  virtual void edResetExpt();
  //@}

  ///
  void read(const char* filename);
  ///
  void write(const char* filename) const;

  /** This saves the experiment file and a summary-of-responses file
		under unique filenames. */
  void storeData();

private:
  class Impl;
  friend class Impl;

  Impl* const itsImpl;
};

static const char vcid_exptdriver_h[] = "$Header$";
#endif // !EXPTDRIVER_H_DEFINED
