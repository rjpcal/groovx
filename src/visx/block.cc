///////////////////////////////////////////////////////////////////////
//
// block.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 12:29:34 1999
// written: Thu Mar 30 00:07:13 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_CC_DEFINED
#define BLOCK_CC_DEFINED

#include "block.h"

#include "experiment.h"
#include "rand.h"
#include "response.h"
#include "tlist.h"
#include "trial.h"

#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "util/strings.h"

#include <algorithm>
#include <iostream.h>
#include <strstream.h>
#include <vector>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  Tlist& theTlist = Tlist::theTlist();
  const string_literal ioTag("Block");

  void serializeVecInt(ostream &os, const vector<int>& vec) {
	 char sep = ' ';
	 os << vec.size() << sep << sep;
	 for (size_t i = 0; i < vec.size(); ++i) {
		os << vec[i] << sep;
	 }
	 if (os.fail()) throw OutputError("VecInt");
  }

  void deserializeVecInt(istream &is, vector<int>& vec) {
	 int size;
	 is >> size;
	 if (size < 0) {
		throw InputError("VecInt saw negative value for size");
	 }
	 vec.resize(size, 0);
	 for (int i = 0; i < size; ++i) {
		is >> vec[i];
	 }
	 if (is.fail()) throw InputError("VecInt");
  }

  int charCountVecInt(const vector<int>& vec) {
	 int count = gCharCount<int>(vec.size()) + 1;
	 for (size_t i = 0; i < vec.size(); ++i) {
		count += gCharCount<int>(vec[i]);
		++count;
	 }
	 count += 5;// fudge factor
	 return count;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Block::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Block::Impl {
public:
  Impl() :
	 itsTrialSequence(),
	 itsRandSeed(0),
	 itsCurTrialSeqIdx(0),
	 itsVerbose(false)
	 {}

  vector<int> itsTrialSequence; // Ordered sequence of indexes into the Tlist
										  // Also functions as # of completed trials

  int itsRandSeed;				  // Random seed used to create itsTrialSequence
  int itsCurTrialSeqIdx;		  // Index of the current trial
  bool itsVerbose;
};

///////////////////////////////////////////////////////////////////////
//
// Block member functions
//
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Block::Block() :
  itsImpl( new Impl )
{
DOTRACE("Block::Block");
}

Block::~Block() 
{
  delete itsImpl;
}

void Block::addTrial(int trialid, int repeat) {
DOTRACE("Block::addTrial");
  for (int i = 0; i < repeat; ++i) {
	 itsImpl->itsTrialSequence.push_back(trialid);
  }
};

void Block::addTrials(int first_trial, int last_trial, int repeat) {
DOTRACE("Block::addTrials");

  // Account for the convention of using -1 to indicate 'beginning'
  // for first_trial, and 'end' for last_trial.

  if (first_trial == -1)
	 first_trial = 0;

  if (last_trial  == -1 ||
		last_trial > theTlist.capacity())
	 last_trial  = theTlist.capacity();

  while (first_trial < last_trial)
	 {
		if (theTlist.isValidId(first_trial))
		  addTrial(first_trial, repeat);
		++first_trial;
	 }
}

void Block::shuffle(int seed) {
DOTRACE("Block::shuffle");
  itsImpl->itsRandSeed = seed;

  Urand generator(seed);
  
  random_shuffle(itsImpl->itsTrialSequence.begin(),
					  itsImpl->itsTrialSequence.end(),
					  generator);
}

void Block::removeAllTrials() {
DOTRACE("Block::removeAllTrials");
  itsImpl->itsTrialSequence.clear();
  itsImpl->itsCurTrialSeqIdx = 0;
}

void Block::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Block::serialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  // itsImpl->itsTrialSequence
  serializeVecInt(os, itsImpl->itsTrialSequence);
  os << endl;
  // itsImpl->itsRandSeed
  os << itsImpl->itsRandSeed << endl;
  // itsImpl->itsCurTrialSeqIdx
  os << itsImpl->itsCurTrialSeqIdx << endl;
  // itsImpl->itsVerbose
  os << itsImpl->itsVerbose << endl;

  if (os.fail()) throw OutputError(ioTag.c_str());
}

void Block::deserialize(istream &is, IOFlag flag) {
DOTRACE("Block::deserialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, ioTag.c_str()); }
  
  // itsImpl->itsTrialSequence
  deserializeVecInt(is, itsImpl->itsTrialSequence);

  // itsImpl->itsRandSeed
  is >> itsImpl->itsRandSeed;
  // itsImpl->itsCurTrialSeqIdx
  is >> itsImpl->itsCurTrialSeqIdx;
  if (itsImpl->itsCurTrialSeqIdx < 0 ||
		size_t(itsImpl->itsCurTrialSeqIdx) > itsImpl->itsTrialSequence.size()) {
	 throw IoValueError(ioTag.c_str());
  }
  // itsImpl->itsVerbose
  int val;
  is >> val;
  itsImpl->itsVerbose = bool(val);

  is.ignore(1, '\n');
  if (is.fail()) throw InputError(ioTag.c_str());
}

int Block::charCount() const {
  return (ioTag.length() + 1
			 + charCountVecInt(itsImpl->itsTrialSequence) + 1
			 + gCharCount<int>(itsImpl->itsRandSeed) + 1
			 + gCharCount<int>(itsImpl->itsCurTrialSeqIdx) + 1
			 + gCharCount<bool>(itsImpl->itsVerbose) + 1
			 + 5); //fudge factor
}

void Block::readFrom(Reader* reader) {
DOTRACE("Block::readFrom");
  itsImpl->itsTrialSequence.clear();
  ReadUtils::readValueSeq(
		 reader, "trialSeq", back_inserter(itsImpl->itsTrialSequence), (int*)0);
  reader->readValue("randSeed", itsImpl->itsRandSeed);
  reader->readValue("curTrialSeqdx", itsImpl->itsCurTrialSeqIdx);
  reader->readValue("verbose", itsImpl->itsVerbose);
}

void Block::writeTo(Writer* writer) const {
DOTRACE("Block::writeTo");

  WriteUtils::writeValueSeq(
       writer, "trialSeq",
		 itsImpl->itsTrialSequence.begin(), itsImpl->itsTrialSequence.end());
  writer->writeValue("randSeed", itsImpl->itsRandSeed);
  writer->writeValue("curTrialSeqdx", itsImpl->itsCurTrialSeqIdx);
  writer->writeValue("verbose", itsImpl->itsVerbose);
}

///////////////
// accessors //
///////////////

Trial& Block::getCurTrial() const {
DOTRACE("Block::getCurTrial");
  return *(theTlist.getCheckedPtr(currentTrial()));
}

int Block::numTrials() const {
DOTRACE("Block::numTrials");
  return itsImpl->itsTrialSequence.size();
}

int Block::numCompleted() const {
DOTRACE("Block::numCompleted");
  return itsImpl->itsCurTrialSeqIdx;
}

int Block::currentTrial() const {
DOTRACE("Block::currentTrial");
  if (isComplete()) return -1;

  return itsImpl->itsTrialSequence[itsImpl->itsCurTrialSeqIdx];
}

int Block::currentTrialType() const {
DOTRACE("Block::currentTrialType");
  if (isComplete()) return -1;

  DebugEval(currentTrial());
  DebugEvalNL(getCurTrial().trialType());

  return getCurTrial().trialType();
}

int Block::prevResponse() const {
DOTRACE("Block::prevResponse");

#ifdef PICKY_DEBUG
  DebugEval(itsImpl->itsCurTrialSeqIdx);
  DebugEvalNL(itsImpl->itsTrialSequence.size());
#endif

  if (itsImpl->itsCurTrialSeqIdx == 0 ||
		itsImpl->itsTrialSequence.size() == 0) return -1;

  return theTlist.getCheckedPtr(
     itsImpl->itsTrialSequence[itsImpl->itsCurTrialSeqIdx-1])->lastResponse();
}

bool Block::isComplete() const {
DOTRACE("Block::isComplete");

#ifdef PICKY_DEBUG
  DebugEval(itsImpl->itsCurTrialSeqIdx);
  DebugEvalNL(itsImpl->itsTrialSequence.size());
#endif

  // This is 'tricky'. The problem is that itsImpl->itsCurTrialSeqIdx may
  // temporarily be negative in between an abortTrial and the
  // corresponding endTrial. This means that we can't only compare
  // itsImpl->itsCurTrialSeqIdx with itsImpl->itsTrialSequence.size(), because the
  // former is signed and the latter is unsigned, forcing a
  // 'promotion' to unsigned of the former... this makes it a huge
  // positive number if it was actually negative. Thus, we must also
  // check that itsImpl->itsCurTrialSeqIdx is actually non-negative before
  // returning 'true' from this function.
  return ((itsImpl->itsCurTrialSeqIdx >= 0) &&
			 (size_t(itsImpl->itsCurTrialSeqIdx) >= itsImpl->itsTrialSequence.size()));
}

const char* Block::trialDescription() const {
DOTRACE("Block::trialDescription");
  if (isComplete()) return "block is complete";

  const int BUF_SIZE = 200;
  static char buf[BUF_SIZE];	  // static because the address is returned

  ostrstream ost(buf, BUF_SIZE);
  ost << "trial id == " << currentTrial() << ", ";
  ost << getCurTrial().description();
  ost << ", completed " << numCompleted()
		<< " of " << numTrials();
  ost << '\0';

  return buf;
}

bool Block::getVerbose() const {
DOTRACE("Block::getVerbose");
  return itsImpl->itsVerbose;
}

void Block::setVerbose(bool val) {
DOTRACE("Block::setVerbose");
  itsImpl->itsVerbose = val;
}

///////////////////////////////////////////////////////////////////////
//
// actions 
//
///////////////////////////////////////////////////////////////////////

void Block::beginTrial(Experiment*) {
DOTRACE("Block::beginTrial");
  if (itsImpl->itsVerbose) {
	 cerr << trialDescription() << endl;
  }
}

void Block::drawTrial(Experiment* expt) {
DOTRACE("Block::drawTrial");
  if (isComplete()) return;

  getCurTrial().trDraw(*(expt->getCanvas()), true);

  expt->edSetCurrentTrial(currentTrial());
}

void Block::undrawTrial(Experiment* expt) {
DOTRACE("Block::undrawTrial");
  if (isComplete()) return;

  getCurTrial().trUndraw(*(expt->getCanvas()), true);
}

void Block::abortTrial(Experiment*) {
DOTRACE("Block::abortTrial");
  if (isComplete()) return;

  // Remember the trial that we are about to abort so we can store it
  // at the end of the sequence.
  int aborted_trial = currentTrial();

  // Erase the aborted trial from the sequence. Subsequent trials will
  // slide up to fill in the gap.
  itsImpl->itsTrialSequence.erase(itsImpl->itsTrialSequence.begin()+itsImpl->itsCurTrialSeqIdx);

  // Add the aborted trial to the back of the sequence.
  itsImpl->itsTrialSequence.push_back(aborted_trial);

  // We must decrement itsImpl->itsCurTrialSeqIdx, so that when it is
  // incremented by endTrial, the next trial has slid into the
  // position where the aborted trial once was.
  --itsImpl->itsCurTrialSeqIdx;
}

void Block::processResponse(const Response& response, Experiment*) {
DOTRACE("Block::processResponse");
  if (isComplete()) return;

  getCurTrial().recordResponse(response);

  if (itsImpl->itsVerbose) {
    cerr << "response: " << response << '\n';
  }
}

void Block::endTrial(Experiment*) {
DOTRACE("Block::endTrial");
  if (isComplete()) return;

  // Prepare to start next trial.
  ++itsImpl->itsCurTrialSeqIdx;
}

void Block::haltExpt(Experiment* expt) {
DOTRACE("Block::haltExpt");
  undrawTrial(expt);
  abortTrial(expt);
  endTrial(expt);
}

void Block::undoPrevTrial() {
DOTRACE("Block::undoPrevTrial");

  DebugEval(itsImpl->itsCurTrialSeqIdx); 

  // Check to make sure we've completed at least one trial
  if (itsImpl->itsCurTrialSeqIdx < 1) return;
  
  // Move the counter back to the previous trial...
  --itsImpl->itsCurTrialSeqIdx;

  // ...and erase the last response given to that trial
  if ( theTlist.isValidId(currentTrial()) ) {
	 getCurTrial().undoLastResponse();
  }
}

void Block::resetBlock() {
DOTRACE("Block::resetBlock");
  while (itsImpl->itsCurTrialSeqIdx > 0) {
	 undoPrevTrial();
  }
}

static const char vcid_block_cc[] = "$Header$";
#endif // !BLOCK_CC_DEFINED
