///////////////////////////////////////////////////////////////////////
// expt.h
// Rob Peters
// created: Sat Mar 13 17:55:27 1999
// written: Wed May 19 14:35:41 1999
// $Id$
//
// This file defines the class Expt. Expt contains a Tlist (itself a
// container of Trial's), and in addition holds a sequence of trial
// id's, used as indices into the Tlist. The functionality of Expt is
// straightforward: once a Tlist has been properly constructed, the
// Expt is run by alternately calling beginTrial() and one of
// recordResponse() or abortTrial(). Both recordResponse() and
// abortTrial() prepare the Expt for the next Trial; thus no sort of
// "nextTrial" call is required.
///////////////////////////////////////////////////////////////////////

#ifndef EXPT_H_DEFINED
#define EXPT_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

#include <vector>
#include <string>

class Tlist;
struct timeval;

///////////////////////////////////////////////////////////////////////
// Expt class
///////////////////////////////////////////////////////////////////////

class Expt : public virtual IO {
public:
  //////////////
  // creators //
  //////////////

  Expt(Tlist& tlist, int repeat, int seed = 0, const string& date="", 
		 const string& host="", const string& subject="");
  virtual ~Expt() {}

  void init(int repeat, int seed = 0, const string& date="", 
				const string& host="", const string& subject="");

  // These I/O functions write/read the _entire_ state of the
  // Expt, including itsTlist (which is only held by reference).
  // In addition, since Tlist itself writes/reads the ObjList and
  // PosList that it holds by reference, these I/O functions are all
  // that is needed to manage Expt's through files.
  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  ///////////////
  // accessors //
  ///////////////

  Tlist& getTlist() { return itsTlist; }
  const Tlist& getTlist() const { return itsTlist; }

  virtual int numTrials() const;
  virtual int numCompleted() const;
  virtual int currentTrial() const;
  virtual int currentTrialType() const;
  virtual int prevResponse() const;
  virtual bool isComplete() const;
  virtual const char* trialDescription() const;

  virtual bool needAutosave() const;

  virtual const string& getEndDate() const { return itsEndDate; }
  virtual const string& getAutosaveFile() const { return itsAutosaveFile; }

  virtual bool getVerbose() const { return itsVerbose; }

  virtual int getAbortWait() const { return itsAbortWait; }
  virtual int getAutosavePeriod() const { return itsAutosavePeriod; }
  virtual int getInterTrialInterval() const { return itsInterTrialInterval; }
  virtual int getStimDur() const { return itsStimDur; }
  virtual int getTimeout() const { return itsTimeout; }

  //////////////////
  // manipulators //
  //////////////////

  virtual void setEndDate(const string& str) { itsEndDate = str; }
  virtual void setAutosaveFile(const string& str) { itsAutosaveFile = str; }
  virtual void setKeyRespPairs(const string& str) { itsKeyRespPairs = str; }

  virtual void setUseFeedback(bool val) { itsUseFeedback = val; }
  virtual void setVerbose(bool val) { itsVerbose = val; }

  virtual void setAbortWait(int val) { itsAbortWait = val; }
  virtual void setAutosavePeriod(int val) { itsAutosavePeriod = val; }
  virtual void setInterTrialInterval(int val) { itsInterTrialInterval = val; }
  virtual void setStimDur(int val) { itsStimDur = val; }
  virtual void setTimeout(int val) { itsTimeout = val; }

  /////////////
  // actions //
  /////////////

  virtual void beginTrial();
  virtual void undrawTrial();
  virtual void abortTrial();
  virtual void recordResponse(int val);
  virtual void undoPrevTrial();

private:
  Tlist& itsTlist;				  // Reference to a container of Trial`s
  vector<int> itsTrialSequence; // Ordered sequence of indexes into itsTlist
										  // Also functions as # of completed trials

  int itsRandSeed;				  // Random seed used to create itsTrialSequence
  int itsCurTrialSeqIdx;        // Index of the current trial

  string itsBeginDate;			  // Date(+time) when Expt was begun
  string itsHostname;			  // Host computer on which Expt was begun
  string itsSubject;				  // Id of subject on whom Expt was performed

  // These members are in search of a better home...
  string itsEndDate;				  // Date(+time) when Expt was stopped
  string itsAutosaveFile;		  // Filename used for autosaves
  string itsKeyRespPairs;		  // Regexp string holding key-response pairs

  bool itsUseFeedback;
  bool itsVerbose;
  int itsAbortWait;
  int itsAutosavePeriod;		  // # of trials between autosaves
  int itsInterTrialInterval;
  int itsStimDur;
  int itsTimeout;

  mutable timeval itsBeginTime; // Used to record the start time of each Trial
};

static const char vcid_expt_h[] = "$Header$";
#endif // !EXPT_H_DEFINED
