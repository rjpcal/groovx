///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Wed Jun 16 19:12:28 1999
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

struct Tcl_Interp;

class Expt;
class ResponseHandler;
class TimingHandler;

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
  static ExptDriver theInstance;
public:
  static ExptDriver& theExptDriver();

  virtual ~ExptDriver();

  void init(int repeat, int seed, const string& date,
				const string& host, const string& subject);
  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  // Accessors + Manipulators
  Tcl_Interp* getInterp();
  void setInterp(Tcl_Interp* interp);

  Expt& expt();
  ResponseHandler& responseHandler();
  TimingHandler& timingHandler();

  const Expt& expt() const;
  const ResponseHandler& responseHandler() const;
  const TimingHandler& timingHandler() const;

  bool needAutosave() const;

  void setBlock(int blockid) { itsBlockId = blockid; }
  void setResponseHandler(int rhid);
  void setTimingHandler(int thid);

  const string& getEndDate() const;
  const string& getAutosaveFile() const;
  bool getVerbose() const;

  void setEndDate(const string& str);
  void setAutosaveFile(const string& str);
  void setVerbose(bool val);

  // Actions
  void edBeginTrial();
  void draw();
  void undraw();
  void edAbortTrial();
  void edEndTrial();
  void edResponseSeen();
  void edProcessResponse(int response);
  void edHaltExpt();

  int write(const char* filename);
  int writeAndExit();

private:
  Tcl_Interp* itsInterp;

  string itsEndDate;				  // Date(+time) when Expt was stopped
  string itsAutosaveFile;		  // Filename used for autosaves

  int itsBlockId;
  int itsRhId;
  int itsThId;
};


static const char vcid_exptdriver_h[] = "$Header$";
#endif // !EXPTDRIVER_H_DEFINED
