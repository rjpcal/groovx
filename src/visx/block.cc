///////////////////////////////////////////////////////////////////////
//
// block.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 12:29:34 1999
// written: Fri Sep 29 14:45:47 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_CC_DEFINED
#define BLOCK_CC_DEFINED

#include "block.h"

#include "experiment.h"
#include "response.h"
#include "tlist.h"
#include "trialbase.h"

#include "io/iolegacy.h"
#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "util/rand.h"
#include "util/strings.h"

#include <algorithm>
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
}

///////////////////////////////////////////////////////////////////////
//
// Block::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Block::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl() :
	 itsTrialSequence(),
	 itsRandSeed(0),
	 itsCurTrialSeqIdx(0),
	 itsVerbose(false),
	 itsCurrentTrialId(-1),
	 itsHasBegun(false),
	 itsExperiment(0)
	 {}

  std::vector<int> itsTrialSequence; // Ordered sequence of indexes into the Tlist

  int itsRandSeed;				  // Random seed used to create itsTrialSequence
  int itsCurTrialSeqIdx;		  // Index of the current trial
										  // Also functions as # of completed trials
  bool itsVerbose;

  int itsCurrentTrialId;

  mutable bool itsHasBegun;

  void updateCurrentTrial()
	 {
		if ( itsCurTrialSeqIdx < 0 || 
			  (unsigned int) itsCurTrialSeqIdx >= itsTrialSequence.size() )
		  itsCurrentTrialId = -1;
		else
		  itsCurrentTrialId = itsTrialSequence.at(itsCurTrialSeqIdx);
	 }

  void setExpt(Experiment& expt)
	 {
		itsExperiment = &expt;
	 }

  Experiment& getExpt()
	 {
		Assert( itsExperiment != 0 );
		return *itsExperiment;
	 }

private:
  mutable Experiment* itsExperiment;
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
  itsImpl->updateCurrentTrial();
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

  Util::Urand generator(seed);
  
  std::random_shuffle(itsImpl->itsTrialSequence.begin(),
							 itsImpl->itsTrialSequence.end(),
							 generator);
  itsImpl->updateCurrentTrial();
}

void Block::removeAllTrials() {
DOTRACE("Block::removeAllTrials");
  itsImpl->itsTrialSequence.clear();
  itsImpl->itsCurTrialSeqIdx = 0;
  itsImpl->itsCurrentTrialId = -1;
}

void Block::legacySrlz(IO::LegacyWriter* writer) const {
DOTRACE("Block::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {

	 IO::WriteUtils::writeValueSeq(
       writer, "trialSeq",
		 itsImpl->itsTrialSequence.begin(), itsImpl->itsTrialSequence.end());

	 lwriter->insertChar('\n');

	 lwriter->setFieldSeparator('\n');
	 writer->writeValue("randSeed", itsImpl->itsRandSeed);
	 writer->writeValue("curTrialSeqdx", itsImpl->itsCurTrialSeqIdx);
	 writer->writeValue("verbose", itsImpl->itsVerbose);
  }
}

void Block::legacyDesrlz(IO::LegacyReader* reader) {
DOTRACE("Block::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {

	 itsImpl->itsTrialSequence.clear();
	 IO::ReadUtils::template readValueSeq<int>(
		 reader, "trialSeq", std::back_inserter(itsImpl->itsTrialSequence));

	 reader->readValue("randSeed", itsImpl->itsRandSeed);

	 reader->readValue("curTrialSeqdx", itsImpl->itsCurTrialSeqIdx);
	 if (itsImpl->itsCurTrialSeqIdx < 0 ||
		  size_t(itsImpl->itsCurTrialSeqIdx) > itsImpl->itsTrialSequence.size()) {
		throw IO::ValueError(ioTag.c_str());
	 }
	 itsImpl->updateCurrentTrial();

	 reader->readValue("verbose", itsImpl->itsVerbose);

	 // XXX I think this is not necessary
	 //  	 lreader->input().ignore(1, '\n');
  }
}

void Block::readFrom(IO::Reader* reader) {
DOTRACE("Block::readFrom");

  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 legacyDesrlz(lreader);
	 return;
  }

  itsImpl->itsTrialSequence.clear();
  IO::ReadUtils::template readValueSeq<int>(
		 reader, "trialSeq", std::back_inserter(itsImpl->itsTrialSequence));
  reader->readValue("randSeed", itsImpl->itsRandSeed);
  reader->readValue("curTrialSeqdx", itsImpl->itsCurTrialSeqIdx);
  if (itsImpl->itsCurTrialSeqIdx < 0 ||
		size_t(itsImpl->itsCurTrialSeqIdx) > itsImpl->itsTrialSequence.size()) {
	 throw IO::ValueError(ioTag.c_str());
  }
  itsImpl->updateCurrentTrial();
  reader->readValue("verbose", itsImpl->itsVerbose);
}

void Block::writeTo(IO::Writer* writer) const {
DOTRACE("Block::writeTo");

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 legacySrlz(lwriter);
	 return;
  }

  IO::WriteUtils::writeValueSeq(
       writer, "trialSeq",
		 itsImpl->itsTrialSequence.begin(), itsImpl->itsTrialSequence.end());
  writer->writeValue("randSeed", itsImpl->itsRandSeed);
  writer->writeValue("curTrialSeqdx", itsImpl->itsCurTrialSeqIdx);
  writer->writeValue("verbose", itsImpl->itsVerbose);
}

///////////////
// accessors //
///////////////

TrialBase& Block::getCurTrial() const {
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

  DebugEvalNL(itsImpl->itsCurrentTrialId);
  return itsImpl->itsCurrentTrialId;
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
     itsImpl->itsTrialSequence.at(itsImpl->itsCurTrialSeqIdx-1))->lastResponse();
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

void Block::beginTrial(Experiment& expt) {
DOTRACE("Block::beginTrial");

  if ( isComplete() ) return;

  itsImpl->itsHasBegun = true;

  if (itsImpl->itsVerbose) {
	 cerr << trialDescription() << endl;
  }

  itsImpl->setExpt(expt);

  getCurTrial().trDoTrial(*(expt.getWidget()), expt.getErrorHandler(), *this);
}

void Block::drawTrialHook() {
DOTRACE("Block::drawTrialHook");
  if (isComplete()) return;

  itsImpl->getExpt().edSetCurrentTrial(currentTrial());
}

void Block::abortTrial() {
DOTRACE("Block::abortTrial");
  if (isComplete()) return;

  // Remember the trial that we are about to abort so we can store it
  // at the end of the sequence.
  int aborted_trial = currentTrial();

  // Erase the aborted trial from the sequence. Subsequent trials will
  // slide up to fill in the gap.
  itsImpl->itsTrialSequence.erase(
				  itsImpl->itsTrialSequence.begin()+itsImpl->itsCurTrialSeqIdx);

  // Add the aborted trial to the back of the sequence.
  itsImpl->itsTrialSequence.push_back(aborted_trial);

  // We must decrement itsImpl->itsCurTrialSeqIdx, so that when it is
  // incremented by endTrial, the next trial has slid into the
  // position where the aborted trial once was.
  --itsImpl->itsCurTrialSeqIdx;
}

void Block::processResponse(const Response& response) {
DOTRACE("Block::processResponse");
  if (isComplete()) return;

  if (itsImpl->itsVerbose) {
    cerr << "response: " << response << '\n';
  }

  DebugEval(response.correctVal());
  DebugEvalNL(response.val());

  if (!response.isCorrect()) {
	 // If the response was incorrect, add a repeat of the current
	 // trial to the block and reshuffle
	 addTrial(currentTrial(), 1);
	 std::random_shuffle(
		itsImpl->itsTrialSequence.begin()+itsImpl->itsCurTrialSeqIdx+1,
		itsImpl->itsTrialSequence.end());
  }
}

void Block::endTrial() {
DOTRACE("Block::endTrial");
  if (isComplete()) return;

  // Prepare to start next trial.
  ++itsImpl->itsCurTrialSeqIdx;
  itsImpl->updateCurrentTrial();

  DebugEval(numCompleted());
  DebugEval(numTrials());
  DebugEvalNL(isComplete());

  itsImpl->getExpt().edEndTrial();
}

void Block::nextTrial() {
DOTRACE("Block::nextTrial");
  if ( !isComplete() ) {
	 beginTrial(itsImpl->getExpt());
  }
  else {
	 itsImpl->getExpt().edNextBlock();
  }
}


void Block::haltExpt() {
DOTRACE("Block::haltExpt");

  if ( itsImpl->itsHasBegun && !isComplete() )
	 getCurTrial().trHaltExpt();
}

void Block::undoPrevTrial() {
DOTRACE("Block::undoPrevTrial");

  DebugEval(itsImpl->itsCurTrialSeqIdx); 

  // Check to make sure we've completed at least one trial
  if (itsImpl->itsCurTrialSeqIdx < 1) return;
  
  // Move the counter back to the previous trial...
  --itsImpl->itsCurTrialSeqIdx;
  itsImpl->updateCurrentTrial();

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
