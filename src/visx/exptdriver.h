///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Wed Nov 17 18:22:44 1999
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

#ifndef TIME_H_DEFINED
#include <sys/time.h>
#define TIME_H_DEFINED
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
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

class ExptDriver : public virtual IO {
protected:
  /// Default constructor
  ExptDriver();

private:
  /// Copy constructor not allowed
  ExptDriver(const ExptDriver&);
  /// assignment not allowed
  ExptDriver& operator=(const ExptDriver&);

  ///
  static ExptDriver theInstance;

public:
  /// Retrieve the singleton instance.
  static ExptDriver& theExptDriver();

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
  void draw();
  ///
  void undraw();
  ///
  void edSwapBuffers();
  //@}

  /** @name Trial event sequence actions */
  //@{
  ///
  void edBeginExpt();

  ///
  void edBeginTrial();
  ///
  void edResponseSeen();
  ///
  void edProcessResponse(int response);
  ///
  void edAbortTrial();
  ///
  void edEndTrial();
  ///
  void edHaltExpt() const;

  ///
  void edResetExpt();
  //@}

  ///
  void read(const char* filename);
  ///
  void write(const char* filename) const;

  /** This saves the experiment file and a summary-of-responses file
		under unique filenames. */
  void storeData();

private:
  class ExptImpl;
  friend class ExptImpl;

  ExptImpl* const itsImpl;
};

static const char vcid_exptdriver_h[] = "$Header$";
#endif // !EXPTDRIVER_H_DEFINED
