///////////////////////////////////////////////////////////////////////
// trialexpt.cc
// Rob Peters
// created: Sat Mar 13 17:57:49 1999
// written: Tue Mar 16 19:14:12 1999
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
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// TrialExpt member functions
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

TrialExpt::TrialExpt(Tlist& tlist, int repeat, int seed) : 
  itsTlist(tlist), 
  itsTrialSequence(),
  itsRandSeed(seed), 
  itsCurTrialSeqIdx(0)
{
DOTRACE("TrialExpt::TrialExpt");
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
	 itsTrialSequence.push_back(*itr);
	 temp_vec.erase(itr);
  }
}

IOResult TrialExpt::serialize(ostream &os, IOFlag flag) const {
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
  return checkStream(os);
}

IOResult TrialExpt::deserialize(istream &is, IOFlag flag) {
DOTRACE("TrialExpt::deserialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }
  if (flag & IO::TYPENAME) {
	 string name;
	 is >> name;
	 if (name != string(typeid(TrialExpt).name())) { return IO_ERROR; }
  }

  // itsTlist
  itsTlist.deserialize(is, flag);

  // itsTrialSequence
  deserializeVecInt(is, itsTrialSequence);

  // itsRandSeed
  is >> itsRandSeed;
  // itsCurTrialSeqIdx
  is >> itsCurTrialSeqIdx;
  return checkStream(is);
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
  return itsTrialSequence[itsCurTrialSeqIdx];
}

int TrialExpt::currentStimClass() const {
DOTRACE("TrialExpt::currentStimClass");
  const Trial *t = itsTlist.getTrial(itsTrialSequence[itsCurTrialSeqIdx]);
  if (!t) return NULL;
  return t->trialType();
}

int TrialExpt::prevResponse() const {
DOTRACE("TrialExpt::prevResponse");
  const Trial *t = itsTlist.getTrial(itsTrialSequence[itsCurTrialSeqIdx-1]);
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
  const Trial *t = itsTlist.getTrial(itsTrialSequence[itsCurTrialSeqIdx]);
  if (!t) return NULL;

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
  itsCurTrialSeqIdx++;
  if (isComplete()) return;
  // This single call sets the Tlist's current trial and visibility
  // appropriately, then renders the objects and flushes the graphics.
  itsTlist.drawTrial(itsTrialSequence[itsCurTrialSeqIdx]);
}

void TrialExpt::recordResponse(int resp) {
DOTRACE("TrialExpt::recordResponse");
  Trial *t = itsTlist.getTrial(itsTrialSequence[itsCurTrialSeqIdx]);
  if (!t) return;
  t->recordResponse(resp);
}

static const char vcid_trialexpt_cc[] = "$Id$";
#endif // !TRIALEXPT_CC_DEFINED
