///////////////////////////////////////////////////////////////////////
//
// exptdriver.h
// Rob Peters
// created: Tue May 11 13:33:50 1999
// written: Wed May 26 18:46:31 1999
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
public:
  // Creators
  ExptDriver(Expt& expt, Tcl_Interp* interp);
  virtual ~ExptDriver();

  void init(int repeat, int seed, const string& date,
				const string& host, const string& subject);
  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  // Accessors + Manipulators
  Expt& expt() { return itsExpt; }

  bool needAutosave() const;

  const string& getEndDate() const;
  const string& getAutosaveFile() const;
  const string& getKeyRespPairs() const; // delegate to itsResponseHandler
  bool getUseFeedback() const; // delegate to itsResponseHandler
  bool getVerbose() const;

  int getAbortWait() const;
  int getAutosavePeriod() const;
  int getInterTrialInterval() const;
  int getStimDur() const;
  int getTimeout() const;

  void setEndDate(const string& str);
  void setAutosaveFile(const string& str);
  void setUseFeedback(bool val); // delegate to itsResponseHandler
  void setKeyRespPairs(const string& s); // delegate to itsResponseHandler
  void setVerbose(bool val);

  void setAbortWait(int val);
  void setAutosavePeriod(int val);
  void setInterTrialInterval(int val);
  void setStimDur(int val);
  void setTimeout(int val);
  
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
  Expt& itsExpt;
  Tcl_Interp* itsInterp;

  string itsEndDate;				  // Date(+time) when Expt was stopped
  string itsAutosaveFile;		  // Filename used for autosaves

  // These are event sources for ExptDriver
  ResponseHandler& itsResponseHandler; // contained by ref to break dependency
  TimingHandler& itsTimingHandler;
};


static const char vcid_exptdriver_h[] = "$Header$";
#endif // !EXPTDRIVER_H_DEFINED
