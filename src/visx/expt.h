///////////////////////////////////////////////////////////////////////
//
// expt.h
// Rob Peters
// created: Sat Mar 13 17:55:27 1999
// written: Fri Jun 25 13:47:32 1999
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

struct timeval;
class Trial;

///////////////////////////////////////////////////////////////////////
// Expt class
///////////////////////////////////////////////////////////////////////

class Expt : public virtual IO {
public:
  //////////////
  // creators //
  //////////////

  Expt();
  virtual ~Expt() {}

  // Add to the Block all valid trialids between first_trial and
  // last_trial, inclusive. If a boundary is set to -1, then the
  // trialid is not checked against that boundary. Each trialid is
  // added 'repeat' times.
  void addTrials(int first_trial=-1, int last_trial=-1, int repeat=1);

  // Randomly permute the sequence of trialids, using seed as the
  // random seed for the shuffle.
  void shuffle(int seed=0);

  // Clear the Block's list of trialids.
  void reset();

  // These I/O functions write/read the _entire_ state of the Expt,
  // including the global Tlist, ObjList, and PosList.
  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  ///////////////
  // accessors //
  ///////////////
  
  Trial& getCurTrial() const;

  // Returns the total number of trials that will comprise the Expt.
  virtual int numTrials() const;

  // Returns the number of trials that have been successfully
  // completed.  This number will not include trials that have been
  // aborted either due to an invalid response or due to a timeout.
  virtual int numCompleted() const;
  virtual int currentTrial() const;

  // Returns the integer type of the current trial.
  virtual int currentTrialType() const;

  // Returns the last valid (but not necessarily "correct") response
  // that was recorded in the current Expt.
  virtual int prevResponse() const;

  // Returns true if the current experiment is complete (i.e. all
  // trials in the trial sequence have finished successfully), false
  // otherwise.
  virtual bool isComplete() const;

  // Returns a human(experimenter)-readable description of the current
  // trial that shows the trial's id, the trial's type, the id's of
  // the objects that are displayed in the trial, the categories of
  // those objects, and the number of completed trials and number of
  // total trials.
  virtual const char* trialDescription() const;

  virtual bool getVerbose() const { return itsVerbose; }
  virtual void setVerbose(bool val) { itsVerbose = val; }

  /////////////
  // actions //
  /////////////

  virtual void beginTrial();

  virtual void drawTrial();
  virtual void undrawTrial();

  // Each of these three functions effectively ends the current trial,
  // and prepares the Block in some way for the next trial.
  virtual void abortTrial();
  virtual void processResponse(int response);
  virtual void endTrial();

  // The state of the experiment is restored to what it was just prior
  // to the beginning of the most recent successfully completed
  // trial. Thus, the current trial is changed to its previous value,
  // and the response to the most recently successfully completed
  // trial is erased.
  virtual void undoPrevTrial();

private:
  vector<int> itsTrialSequence; // Ordered sequence of indexes into the Tlist
										  // Also functions as # of completed trials

  int itsRandSeed;				  // Random seed used to create itsTrialSequence
  int itsCurTrialSeqIdx;        // Index of the current trial
  bool itsVerbose;

  mutable timeval itsBeginTime; // Used to record the start time of each Trial
};

static const char vcid_expt_h[] = "$Header$";
#endif // !EXPT_H_DEFINED
