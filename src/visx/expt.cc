///////////////////////////////////////////////////////////////////////
// trialexpt.cc
// Rob Peters
// created: Sat Mar 13 17:57:49 1999
// written: Mon Apr 19 12:10:13 1999
// $Header$
///////////////////////////////////////////////////////////////////////

#ifndef TRIALEXPT_CC_DEFINED
#define TRIALEXPT_CC_DEFINED

#include "trialexpt.h"

#include <iostream.h>
#include <strstream.h>
#include <string>
#include <typeinfo>

#include "randutils.h"
#include "iostl.h"
#include "tlist.h"
#include "trial.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// TrialExpt member functions
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

TrialExpt::TrialExpt(Tlist& tlist, int repeat, int seed) : 
  itsTlist(tlist)
{
DOTRACE("TrialExpt::TrialExpt");
  init(repeat, seed);
}

void TrialExpt::init(int repeat, int seed) {
DOTRACE("TrialExpt::init");
  itsTrialSequence.clear();
  itsRandSeed = seed;
  itsCurTrialSeqIdx = 0;

  // obtain a list of all valid trial id's from itsTlist
  vector<int> id_vec;
  itsTlist.getValidTrials(id_vec);

  // push all the trial id's that will be used in TrialExpt onto a
  // vector, in order
  vector<int> temp_vec;
  for (int i = 0; i < id_vec.size(); i++) {
	 for (int j = 0; j < repeat; j++) {
		temp_vec.push_back(id_vec[i]);
	 }
  }

  // pull the trial id's out of temp_vec in random order, pushing them
  // onto itsTrialSequence
  srand(seed);
  vector<int>::iterator itr;
  while ( !temp_vec.empty() ) {
	 itr = temp_vec.begin() + randIntRange(0, temp_vec.size());
#ifdef LOCAL_DEBUG
	 DUMP_VAL1(temp_vec.size());
	 DUMP_VAL2(itr-temp_vec.begin());
#endif
	 itsTrialSequence.push_back(*itr);
	 temp_vec.erase(itr);
  }
}

void TrialExpt::serialize(ostream &os, IOFlag flag) const {
DOTRACE("TrialExpt::serialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(TrialExpt).name() << sep; }

  // itsTlist
  itsTlist.serialize(os, flag);

  // itsTrialSequence
  serializeVecInt(os, itsTrialSequence);

  // itsRandSeed
  os << itsRandSeed << sep;
  // itsCurTrialSeqIdx
  os << itsCurTrialSeqIdx << sep;
  if (os.fail()) throw OutputError(typeid(TrialExpt));
}

void TrialExpt::deserialize(istream &is, IOFlag flag) {
DOTRACE("TrialExpt::deserialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }
  if (flag & IO::TYPENAME) {
	 string name;
	 is >> name;
	 if (name != string(typeid(TrialExpt).name())) { 
		throw InputError(typeid(TrialExpt));
	 }
  }

  // itsTlist
  itsTlist.deserialize(is, flag);

  // itsTrialSequence
  deserializeVecInt(is, itsTrialSequence);

  // itsRandSeed
  is >> itsRandSeed;
  // itsCurTrialSeqIdx
  is >> itsCurTrialSeqIdx;
  if (itsCurTrialSeqIdx < 0 || itsCurTrialSeqIdx > itsTrialSequence.size()) {
	 throw IoValueError(typeid(TrialExpt));
  }
  if (is.fail()) throw InputError(typeid(TrialExpt));
}

///////////////
// accessors //
///////////////

int TrialExpt::numTrials() const {
DOTRACE("TrialExpt::numTrials");
  return itsTrialSequence.size();
}

int TrialExpt::numCompleted() const {
DOTRACE("TrialExpt::numCompleted");
  return itsCurTrialSeqIdx;
}

int TrialExpt::currentTrial() const {
DOTRACE("TrialExpt::currentTrial");
  if (isComplete()) return -1;

  return itsTrialSequence[itsCurTrialSeqIdx];
}

int TrialExpt::currentStimClass() const {
DOTRACE("TrialExpt::currentStimClass");
  if (isComplete()) return -1;

  const Trial *t = itsTlist.getTrial(itsTrialSequence[itsCurTrialSeqIdx]);
  Assert(t);
  return t->trialType();
}

int TrialExpt::prevResponse() const {
DOTRACE("TrialExpt::prevResponse");
#ifdef LOCAL_DEBUG
  DUMP_VAL1(itsCurTrialSeqIdx);
  DUMP_VAL2(itsTrialSequence.size());
#endif
  if (itsCurTrialSeqIdx == 0 || itsTrialSequence.size() == 0) return -1;

  const Trial *t = itsTlist.getTrial(itsTrialSequence[itsCurTrialSeqIdx-1]);
  Assert(t);
  return t->lastResponse();
}

bool TrialExpt::isComplete() const {
DOTRACE("TrialExpt::isComplete");
#ifdef LOCAL_DEBUG
  DUMP_VAL1(itsCurTrialSeqIdx);
  DUMP_VAL2(itsTrialSequence.size());
#endif
  return (itsCurTrialSeqIdx >= itsTrialSequence.size());
}

const char* TrialExpt::trialDescription() const {
DOTRACE("TrialExpt::trialDescription");
  if (isComplete()) return "expt is complete";

  const Trial *t = itsTlist.getTrial(itsTrialSequence[itsCurTrialSeqIdx]);
  Assert(t);

  const int BUF_SIZE = 200;
  static char buf[BUF_SIZE];	  // static because the address is returned

  ostrstream ost(buf, BUF_SIZE);
  ost << "trial id == " << itsTrialSequence[itsCurTrialSeqIdx] << ", ";
  ost << t->description();
  ost << ", completed " << numCompleted()
		<< " of " << numTrials();
  ost << '\0';

  return buf;
}


/////////////
// actions //
/////////////

void TrialExpt::beginTrial() {
DOTRACE("TrialExpt::beginTrial");
  if (isComplete()) return;

  // This single call sets the Tlist's current trial and visibility
  // appropriately, then renders the objects and flushes the graphics.
  itsTlist.drawTrial(itsTrialSequence[itsCurTrialSeqIdx]);
}

void TrialExpt::abortTrial() {
DOTRACE("TrialExpt::abortTrial");
  if (isComplete()) return;

  // Remember the trial that we are about to abort so we can store it
  // at the end of the sequence.
  int aborted_trial = itsTrialSequence[itsCurTrialSeqIdx];

  // Erase the aborted trial from the sequence. Subsequent trials will
  // slide up to fill in the gap.
  itsTrialSequence.erase(itsTrialSequence.begin()+itsCurTrialSeqIdx);

  // Add the aborted trial to the back of the sequence.
  itsTrialSequence.push_back(aborted_trial);

  // itsCurTrialSeqIdx does not have to change, since the next trial
  // has slid into the position where the aborted trial once was.
}

void TrialExpt::recordResponse(int resp) {
DOTRACE("TrialExpt::recordResponse");
  if (isComplete()) return;

  Trial *t = itsTlist.getTrial(itsTrialSequence[itsCurTrialSeqIdx]);
  Assert(t);
  t->recordResponse(resp);

  // Prepare to start next trial.
  itsCurTrialSeqIdx++;
}

static const char vcid_trialexpt_cc[] = "$Id$";
#endif // !TRIALEXPT_CC_DEFINED
