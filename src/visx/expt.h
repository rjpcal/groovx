///////////////////////////////////////////////////////////////////////
//
// expt.h
// Rob Peters
// created: Sat Mar 13 17:55:27 1999
// written: Wed Jun  9 12:31:53 1999
// $Id$
//
// This file defines the class Expt. Expt holds a sequence of trial
// id's, used as indices into the global singleton Tlist. The Expt is
// run by alternately calling drawTrial() and one of processResponse()
// or abortTrial(). Both processResponse() and abortTrial() prepare
// the Expt for the next Trial; thus no sort of "nextTrial" call is
// required.
//
///////////////////////////////////////////////////////////////////////

#ifndef EXPT_H_DEFINED
#define EXPT_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

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

  Expt(int repeat, int seed = 0, const string& date="", 
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

  virtual int numTrials() const;
  virtual int numCompleted() const;
  virtual int currentTrial() const;
  virtual int currentTrialType() const;
  virtual int prevResponse() const;
  virtual bool isComplete() const;
  virtual const char* trialDescription() const;

  virtual bool getVerbose() const { return itsVerbose; }
  virtual void setVerbose(bool val) { itsVerbose = val; }

  /////////////
  // actions //
  /////////////

  virtual void beginTrial();

  virtual void drawTrial();
  virtual void undrawTrial();
  virtual void abortTrial();
  virtual void processResponse(int response);
  virtual void undoPrevTrial();

private:
  vector<int> itsTrialSequence; // Ordered sequence of indexes into itsTlist
										  // Also functions as # of completed trials

  int itsRandSeed;				  // Random seed used to create itsTrialSequence
  int itsCurTrialSeqIdx;        // Index of the current trial

  string itsBeginDate;			  // Date(+time) when Expt was begun
  string itsHostname;			  // Host computer on which Expt was begun
  string itsSubject;				  // Id of subject on whom Expt was performed

  mutable timeval itsBeginTime; // Used to record the start time of each Trial

  // This is serialized elsewhere for legacy reasons
  bool itsVerbose;
};

static const char vcid_expt_h[] = "$Header$";
#endif // !EXPT_H_DEFINED
