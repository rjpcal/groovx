///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Wed Nov  3 14:47:31 1999
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
//
// ExptDriver class defintion
//
///////////////////////////////////////////////////////////////////////

class ExptDriver : public virtual IO {
protected:
  // Creators
  ExptDriver();

private:
  ExptDriver(const ExptDriver&);	// copy constructor not allowed
  ExptDriver& operator=(const ExptDriver&); // assignment not allowed

  static ExptDriver theInstance;

public:
  static ExptDriver& theExptDriver();

  virtual ~ExptDriver();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  // Accessors + Manipulators
  Tcl_Interp* getInterp();
  void setInterp(Tcl_Interp* interp);

  const string& getAutosaveFile() const;
  void setAutosaveFile(const string& str);

  // Graphics actions
  void draw();
  void undraw();
  void edSwapBuffers();

  // Trial event sequence actions
  void edBeginExpt();

  void edBeginTrial();
  void edResponseSeen();
  void edProcessResponse(int response);
  void edAbortTrial();
  void edEndTrial();
  void edHaltExpt() const;

  void edResetExpt();

  void read(const char* filename);
  void write(const char* filename) const;

  // This saves the experiment file and a summary-of-responses file
  // under unique filenames, and optionally quits the application if
  // quitApplication is true.
  void writeAndExit(bool quitApplication=false);

private:
  class ExptImpl;
  friend class ExptImpl;

  ExptImpl* const itsImpl;
};

static const char vcid_exptdriver_h[] = "$Header$";
#endif // !EXPTDRIVER_H_DEFINED
