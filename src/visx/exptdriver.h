///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Tue Jul 20 14:43:05 1999
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

  void read(const char* filename);
  void write(const char* filename) const;

  // Note: this function calls Tcl_Exit(), so it will never return.
  void writeAndExit();

protected:
  void init();

  Block& block() const;
  ResponseHandler& responseHandler() const;
  TimingHdlr& timingHdlr() const;

  bool needAutosave() const;

  // Check the validity of all its id's, return true if all are ok,
  // otherwise returns false, halts the experiment, and issues an
  // error message to itsInterp.
  bool assertIds() const; 

private:
  Tcl_Interp* itsInterp;

  string itsHostname;			  // Host computer on which Expt was begun
  string itsSubject;				  // Id of subject on whom Expt was performed
  string itsBeginDate;			  // Date(+time) when Expt was begun
  string itsEndDate;				  // Date(+time) when Expt was stopped
  string itsAutosaveFile;		  // Filename used for autosaves

  int itsBlockId;
  int itsRhId;
  int itsThId;

  mutable timeval itsBeginTime;
};

static const char vcid_exptdriver_h[] = "$Header$";
#endif // !EXPTDRIVER_H_DEFINED
