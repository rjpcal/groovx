///////////////////////////////////////////////////////////////////////
// expt.h
// Rob Peters
// created: Sat Mar 13 17:55:27 1999
// written: Sun Apr 25 12:50:04 1999
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
  
  ///////////////
  // accessors //
  ///////////////

  Tlist& getTlist() { return itsTlist; }
  const Tlist& getTlist() const { return itsTlist; }

  virtual int numTrials() const;
  virtual int numCompleted() const;
  virtual int currentTrial() const;
  virtual int currentStimClass() const;
  virtual int prevResponse() const;
  virtual bool isComplete() const;
  virtual const char* trialDescription() const;

  /////////////
  // actions //
  /////////////

  virtual void beginTrial();
  virtual void abortTrial();
  virtual void recordResponse(int val, int msec);

private:
  Tlist& itsTlist;				  // Reference to a container of Trial`s
  vector<int> itsTrialSequence; // Ordered sequence of indexes into itsTlist
										  // Also functions as # of completed trials

  int itsRandSeed;				  // Random seed used to create itsTrialSequence
  int itsCurTrialSeqIdx;        // Index of the current trial

  string itsDate;					  // Date(+time) when Expt was begun
  string itsHostname;			  // Host computer on which Expt was begun
  string itsSubject;				  // Id of subject on whom Expt was performed
};

static const char vcid_expt_h[] = "$Header$";
#endif // !EXPT_H_DEFINED
