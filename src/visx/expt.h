///////////////////////////////////////////////////////////////////////
// trialexpt.h
// Rob Peters
// created: Sat Mar 13 17:55:27 1999
// written: Mon Apr 19 13:03:45 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TRIALEXPT_H_DEFINED
#define TRIALEXPT_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

#include <vector>

class Tlist;

///////////////////////////////////////////////////////////////////////
// TrialExpt class
///////////////////////////////////////////////////////////////////////

class TrialExpt : public virtual IO {
public:
  // creators
  TrialExpt(Tlist& tlist, int repeat, int seed = 0);
  ~TrialExpt() {}

  void init(int repeat, int seed = 0);

  // These I/O functions write/read the _entire_ state of the
  // TrialExpt, including itsTlist (which is only held by reference).
  // In addition, since Tlist itself writes/reads the ObjList and
  // PosList that it holds by reference, these I/O functions are all
  // that is needed to manage Expt's through files.
  virtual void serialize(ostream &os, IOFlag flag = NO_FLAGS) const;
  virtual void deserialize(istream &is, IOFlag flag = NO_FLAGS);
  
  // accessors
  virtual int numTrials() const;
  virtual int numCompleted() const;
  virtual int currentTrial() const;
  virtual int currentStimClass() const;
  virtual int prevResponse() const;
  virtual bool isComplete() const;
  virtual const char* trialDescription() const;

  // actions
  virtual void beginTrial();
  virtual void abortTrial();
  virtual void recordResponse(int resp);

private:
  Tlist& itsTlist;
  vector<int> itsTrialSequence; // ordered list of indexes into itsTlist
                                  // (may contain repeats)
  int itsRandSeed;
  int itsCurTrialSeqIdx;        // index into itsTrialList
};

static const char vcid_trialexpt_h[] = "$Id$";
#endif // !TRIALEXPT_H_DEFINED
