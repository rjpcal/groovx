///////////////////////////////////////////////////////////////////////
//
// block.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 12:29:34 1999
// written: Thu Oct 26 17:14:18 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_CC_DEFINED
#define BLOCK_CC_DEFINED

#include "block.h"

#include "experiment.h"
#include "itemwithid.h"
#include "response.h"
#include "trialbase.h"

#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "util/rand.h"
#include "util/strings.h"

#include <algorithm>
#include <strstream.h>
#include <vector>

#define DYNAMIC_TRACE_EXPR Block::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

Util::Tracer Block::tracer;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string_literal ioTag("Block");

  IO::VersionId BLOCK_SERIAL_VERSION_ID = 1;
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
	 itsCurrentTrial(-1),
	 itsHasBegun(false),
	 itsExperiment(0)
	 {}

  // Ordered sequence of indexes into the Tlist
  std::vector<NullableItemWithId<TrialBase> > itsTrialSequence;

  int itsRandSeed;				  // Random seed used to create itsTrialSequence
  int itsCurTrialSeqIdx;		  // Index of the current trial
										  // Also functions as # of completed trials
  bool itsVerbose;

  NullableItemWithId<TrialBase> itsCurrentTrial;

  mutable bool itsHasBegun;

  void updateCurrentTrial()
	 {
		if ( itsCurTrialSeqIdx < 0 || 
			  (unsigned int) itsCurTrialSeqIdx >= itsTrialSequence.size() )
		  itsCurrentTrial = NullableItemWithId<TrialBase>(-1);
		else
		  itsCurrentTrial = itsTrialSequence.at(itsCurTrialSeqIdx);
	 }

  void setExpt(Experiment& expt)
	 {
		Precondition( &expt != 0 );
		itsExperiment = &expt;
	 }

  Experiment& getExpt()
	 {
		Precondition( itsExperiment != 0 );
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

Block* Block::make() {
DOTRACE("Block::make");
  return new Block;
}

Block::Block() :
  itsImpl( new Impl )
{
DOTRACE("Block::Block");
}

Block::~Block() 
{
  delete itsImpl;
}

void Block::addTrial(NullableItemWithId<TrialBase> trial, int repeat) {
DOTRACE("Block::addTrial");
  for (int i = 0; i < repeat; ++i) {
	 itsImpl->itsTrialSequence.push_back(trial);
  }
  itsImpl->updateCurrentTrial();
};

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
  itsImpl->itsCurrentTrial = NullableItemWithId<TrialBase>(-1);
}

IO::VersionId Block::serialVersionId() const {
DOTRACE("Block::serialVersionId");
  return BLOCK_SERIAL_VERSION_ID;
}

void Block::readFrom(IO::Reader* reader) {
DOTRACE("Block::readFrom");

  IO::VersionId svid = reader->readSerialVersionId(); 

  if (svid < 1)
	 {
		throw IO::ReadVersionError("Block", svid, 1, "Try grsh0.8a3");
	 }

  Assert(svid >= 1);

  itsImpl->itsTrialSequence.clear();
  IO::ReadUtils::readObjectSeq<TrialBase>(
        reader, "trialSeq",
		  NullableItemWithId<TrialBase>::makeInserter(itsImpl->itsTrialSequence));

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

  if (BLOCK_SERIAL_VERSION_ID < 1)
	 {
		throw IO::WriteVersionError("Block", BLOCK_SERIAL_VERSION_ID, 1,
											 "Try grsh0.8a3");
	 } 

  Assert(BLOCK_SERIAL_VERSION_ID >= 1);

  IO::WriteUtils::writeSmartPtrSeq(writer, "trialSeq",
											  itsImpl->itsTrialSequence.begin(),
											  itsImpl->itsTrialSequence.end());

  writer->writeValue("randSeed", itsImpl->itsRandSeed);
  writer->writeValue("curTrialSeqdx", itsImpl->itsCurTrialSeqIdx);
  writer->writeValue("verbose", itsImpl->itsVerbose);
}

///////////////
// accessors //
///////////////

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

  DebugEvalNL(itsImpl->itsCurrentTrial.id());
  return itsImpl->itsCurrentTrial.id();
}

int Block::currentTrialType() const {
DOTRACE("Block::currentTrialType");
  if (isComplete()) return -1;

  DebugEval(currentTrial());
  DebugEvalNL(itsImpl->itsCurrentTrial->trialType());

  return itsImpl->itsCurrentTrial->trialType();
}

int Block::prevResponse() const {
DOTRACE("Block::prevResponse");

#ifdef PICKY_DEBUG
  DebugEval(itsImpl->itsCurTrialSeqIdx);
  DebugEvalNL(itsImpl->itsTrialSequence.size());
#endif

  if (itsImpl->itsCurTrialSeqIdx == 0 ||
		itsImpl->itsTrialSequence.size() == 0) return -1;

  NullableItemWithId<TrialBase> prev_trial = 
	 itsImpl->itsTrialSequence.at(itsImpl->itsCurTrialSeqIdx-1);
  return prev_trial->lastResponse();
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
  ost << itsImpl->itsCurrentTrial->description();
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

  itsImpl->itsCurrentTrial->
             trDoTrial(*(expt.getWidget()), expt.getErrorHandler(), *this);
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
  itsImpl->itsTrialSequence.push_back(
                     NullableItemWithId<TrialBase>(aborted_trial));

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
	 addTrial(NullableItemWithId<TrialBase>(currentTrial()), 1);
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
	 itsImpl->itsCurrentTrial->trHaltExpt();
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
  if ( itsImpl->itsCurrentTrial.isValid() ) {
	 itsImpl->itsCurrentTrial->undoLastResponse();
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
