///////////////////////////////////////////////////////////////////////
// expt.cc
// Rob Peters
// created: Sat Mar 13 17:57:49 1999
// written: Sun Apr 25 13:19:33 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef EXPT_CC_DEFINED
#define EXPT_CC_DEFINED

#include "expt.h"

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
// Expt member functions
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Expt::Expt(Tlist& tlist, int repeat, int seed, const string& date, 
			  const string& host, const string& subject) : 
  itsTlist(tlist)
{
DOTRACE("Expt::Expt");
  init(repeat, seed, date, host, subject);
}

void Expt::init(int repeat, int seed, const string& date, 
					 const string& host, const string& subject) {
DOTRACE("Expt::init");
  itsTrialSequence.clear();
  itsRandSeed = seed;
  itsCurTrialSeqIdx = 0;
  itsDate = date;
  itsHostname = host;
  itsSubject = subject;

  // obtain a list of all valid trial id's from itsTlist
  vector<int> id_vec;
  itsTlist.getValidTrials(id_vec);

  // push all the trial id's that will be used in Expt onto a
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

void Expt::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Expt::serialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << typeid(Expt).name() << sep; }

  // itsTlist
  itsTlist.serialize(os, flag);

  // itsTrialSequence
  serializeVecInt(os, itsTrialSequence);
  os << endl;
  // itsRandSeed
  os << itsRandSeed << endl;
  // itsCurTrialSeqIdx
  os << itsCurTrialSeqIdx << endl;
  // itsDate
  os << itsDate << '\n';
  // itsHostname
  os << itsHostname << '\n';
  // itsSubject
  os << itsSubject << '\n';
  if (os.fail()) throw OutputError(typeid(Expt));
}

void Expt::deserialize(istream &is, IOFlag flag) {
DOTRACE("Expt::deserialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) {
	 string name;
	 is >> name;
	 // Must also compare against "TrialExpt" for backward compatibility
	 if (name != typeid(Expt).name() && name != "TrialExpt") { 
		throw InputError(typeid(Expt));
	 }
  }
  
  // restore to default state
  init(0,0);

  // itsTlist
  itsTlist.deserialize(is, flag);

  // itsTrialSequence
  deserializeVecInt(is, itsTrialSequence);

  // itsRandSeed
  is >> itsRandSeed;
  // itsCurTrialSeqIdx
  is >> itsCurTrialSeqIdx;
  is.ignore(1, '\n');
  if (itsCurTrialSeqIdx < 0 || itsCurTrialSeqIdx > itsTrialSequence.size()) {
	 throw IoValueError(typeid(Expt));
  }
  // itsDate
  if (is.peek() != EOF) {
	 getline(is, itsDate, '\n');
  }
  // itsHostname
  if (is.peek() != EOF) {
	 getline(is, itsHostname, '\n');
  }
  // itsSubject
  if (is.peek() != EOF) {
	 getline(is, itsSubject, '\n');
  }
  if (is.fail()) throw InputError(typeid(Expt));
}

///////////////
// accessors //
///////////////

int Expt::numTrials() const {
DOTRACE("Expt::numTrials");
  return itsTrialSequence.size();
}

int Expt::numCompleted() const {
DOTRACE("Expt::numCompleted");
  return itsCurTrialSeqIdx;
}

int Expt::currentTrial() const {
DOTRACE("Expt::currentTrial");
  if (isComplete()) return -1;

  return itsTrialSequence[itsCurTrialSeqIdx];
}

int Expt::currentStimClass() const {
DOTRACE("Expt::currentStimClass");
  if (isComplete()) return -1;

  return itsTlist.getTrial(currentTrial()).trialType();
}

int Expt::prevResponse() const {
DOTRACE("Expt::prevResponse");
#ifdef LOCAL_DEBUG
  DUMP_VAL1(itsCurTrialSeqIdx);
  DUMP_VAL2(itsTrialSequence.size());
#endif
  if (itsCurTrialSeqIdx == 0 || itsTrialSequence.size() == 0) return -1;

  return itsTlist.getTrial(itsTrialSequence[itsCurTrialSeqIdx-1]).lastResponse();
}

bool Expt::isComplete() const {
DOTRACE("Expt::isComplete");
#ifdef LOCAL_DEBUG
  DUMP_VAL1(itsCurTrialSeqIdx);
  DUMP_VAL2(itsTrialSequence.size());
#endif
  return (itsCurTrialSeqIdx >= itsTrialSequence.size());
}

const char* Expt::trialDescription() const {
DOTRACE("Expt::trialDescription");
  if (isComplete()) return "expt is complete";

  const Trial& t = itsTlist.getTrial(currentTrial());

  const int BUF_SIZE = 200;
  static char buf[BUF_SIZE];	  // static because the address is returned

  ostrstream ost(buf, BUF_SIZE);
  ost << "trial id == " << currentTrial() << ", ";
  ost << itsTlist.getTrial(currentTrial()).description();
  ost << ", completed " << numCompleted()
		<< " of " << numTrials();
  ost << '\0';

  return buf;
}


/////////////
// actions //
/////////////

void Expt::beginTrial() {
DOTRACE("Expt::beginTrial");
  if (isComplete()) return;

  // This single call sets the Tlist's current trial and visibility
  // appropriately, then renders the objects and flushes the graphics.
  itsTlist.drawTrial(currentTrial());
}

void Expt::abortTrial() {
DOTRACE("Expt::abortTrial");
  if (isComplete()) return;

  // Remember the trial that we are about to abort so we can store it
  // at the end of the sequence.
  int aborted_trial = currentTrial();

  // Erase the aborted trial from the sequence. Subsequent trials will
  // slide up to fill in the gap.
  itsTrialSequence.erase(itsTrialSequence.begin()+itsCurTrialSeqIdx);

  // Add the aborted trial to the back of the sequence.
  itsTrialSequence.push_back(aborted_trial);

  // itsCurTrialSeqIdx does not have to change, since the next trial
  // has slid into the position where the aborted trial once was.
}

void Expt::recordResponse(int val, int msec) {
DOTRACE("Expt::recordResponse");
  if (isComplete()) return;

  itsTlist.getTrial(currentTrial()).recordResponse(val, msec);

  // Prepare to start next trial.
  itsCurTrialSeqIdx++;
}

static const char vcid_expt_cc[] = "$Header$";
#endif // !EXPT_CC_DEFINED
