///////////////////////////////////////////////////////////////////////
//
// block.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 12:29:34 1999
// written: Wed Aug  8 13:26:00 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_CC_DEFINED
#define BLOCK_CC_DEFINED

#include "block.h"

#include "experiment.h"
#include "response.h"
#include "trialbase.h"

#include "gwt/widget.h"

#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "util/log.h"
#include "util/minivec.h"
#include "util/rand.h"
#include "util/ref.h"
#include "util/strings.h"

#include <algorithm>

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

namespace
{
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
    itsHasBegun(false),
    itsExperiment(0)
    {}

  // Ordered sequence of indexes into the Tlist
  typedef minivec<Ref<TrialBase> > TrialSeqType;
  TrialSeqType itsTrialSequence;

  int itsRandSeed;              // Random seed used to create itsTrialSequence
  int itsCurTrialSeqIdx;        // Index of the current trial
                                // Also functions as # of completed trials
  bool itsVerbose;

  mutable bool itsHasBegun;

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

  Ref<TrialBase> currentTrial()
    {
      Precondition(hasCurrentTrial());

      return itsTrialSequence.at(itsCurTrialSeqIdx);
    }

  bool hasCurrentTrial()
    {
      if ( itsCurTrialSeqIdx < 0 ||
           (unsigned int) itsCurTrialSeqIdx >= itsTrialSequence.size() )
        return false;
      else
        return true;
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

Block* Block::make()
{
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

void Block::addTrial(Ref<TrialBase> trial, int repeat)
{
DOTRACE("Block::addTrial");
  for (int i = 0; i < repeat; ++i)
    {
      itsImpl->itsTrialSequence.push_back(trial);
    }
};

void Block::shuffle(int seed)
{
DOTRACE("Block::shuffle");
  itsImpl->itsRandSeed = seed;

  Util::Urand generator(seed);

  std::random_shuffle(itsImpl->itsTrialSequence.begin(),
                      itsImpl->itsTrialSequence.end(),
                      generator);
}

void Block::removeAllTrials()
{
DOTRACE("Block::removeAllTrials");
  itsImpl->itsTrialSequence.clear();
  itsImpl->itsCurTrialSeqIdx = 0;
}

IO::VersionId Block::serialVersionId() const
{
DOTRACE("Block::serialVersionId");
  return BLOCK_SERIAL_VERSION_ID;
}

void Block::readFrom(IO::Reader* reader)
{
DOTRACE("Block::readFrom");

  reader->ensureReadVersionId("Block", 1, "Try grsh0.8a3");

  itsImpl->itsTrialSequence.clear();
  IO::ReadUtils::readObjectSeq<TrialBase>(
        reader, "trialSeq", std::back_inserter(itsImpl->itsTrialSequence));

  reader->readValue("randSeed", itsImpl->itsRandSeed);
  reader->readValue("curTrialSeqdx", itsImpl->itsCurTrialSeqIdx);
  if (itsImpl->itsCurTrialSeqIdx < 0 ||
      size_t(itsImpl->itsCurTrialSeqIdx) > itsImpl->itsTrialSequence.size())
    {
      throw IO::ReadError("Block");
    }

  reader->readValue("verbose", itsImpl->itsVerbose);
}

void Block::writeTo(IO::Writer* writer) const
{
DOTRACE("Block::writeTo");

  writer->ensureWriteVersionId("Block", BLOCK_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a3");

  IO::WriteUtils::writeObjectSeq(writer, "trialSeq",
                                 itsImpl->itsTrialSequence.begin(),
                                 itsImpl->itsTrialSequence.end());

  writer->writeValue("randSeed", itsImpl->itsRandSeed);
  writer->writeValue("curTrialSeqdx", itsImpl->itsCurTrialSeqIdx);
  writer->writeValue("verbose", itsImpl->itsVerbose);
}

///////////////
// accessors //
///////////////

int Block::numTrials() const
{
DOTRACE("Block::numTrials");
  return itsImpl->itsTrialSequence.size();
}

int Block::numCompleted() const
{
DOTRACE("Block::numCompleted");
  return itsImpl->itsCurTrialSeqIdx;
}

Util::WeakRef<TrialBase> Block::currentTrial() const
{
DOTRACE("Block::currentTrial");
  if (isComplete()) return Util::WeakRef<TrialBase>();

  return itsImpl->currentTrial();
}

int Block::currentTrialType() const
{
DOTRACE("Block::currentTrialType");
  if (isComplete()) return -1;

  DebugEvalNL(itsImpl->currentTrial()->trialType());

  return itsImpl->currentTrial()->trialType();
}

int Block::prevResponse() const
{
DOTRACE("Block::prevResponse");

#ifdef PICKY_DEBUG
  DebugEval(itsImpl->itsCurTrialSeqIdx);
  DebugEvalNL(itsImpl->itsTrialSequence.size());
#endif

  if (itsImpl->itsCurTrialSeqIdx == 0 ||
      itsImpl->itsTrialSequence.size() == 0) return -1;

  Ref<TrialBase> prev_trial =
    itsImpl->itsTrialSequence.at(itsImpl->itsCurTrialSeqIdx-1);
  return prev_trial->lastResponse();
}

bool Block::isComplete() const
{
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

const char* Block::trialDescription() const
{
DOTRACE("Block::trialDescription");
  if (isComplete()) return "block is complete";

  static dynamic_string descr;
  descr = "";
  descr.append("trial id == ").append(currentTrial().id()).append(", ")
    .append(itsImpl->currentTrial()->description())
    .append(", completed ").append(numCompleted())
    .append(" of ").append(numTrials());

  return descr.c_str();
}

bool Block::getVerbose() const
{
DOTRACE("Block::getVerbose");
  return itsImpl->itsVerbose;
}

void Block::setVerbose(bool val)
{
DOTRACE("Block::setVerbose");
  itsImpl->itsVerbose = val;
}

///////////////////////////////////////////////////////////////////////
//
// actions
//
///////////////////////////////////////////////////////////////////////

void Block::beginTrial(Experiment& expt)
{
DOTRACE("Block::beginTrial");

  if ( isComplete() ) return;

  itsImpl->itsHasBegun = true;

  if (itsImpl->itsVerbose)
    {
      Util::log() << trialDescription() << '\n';
    }

  itsImpl->setExpt(expt);

  itsImpl->currentTrial()->
             trDoTrial(expt.getWidget(), expt.getErrorHandler(), *this);
}

void Block::abortTrial()
{
DOTRACE("Block::abortTrial");
  if (isComplete()) return;

  // Remember the trial that we are about to abort so we can store it
  // at the end of the sequence.
  Ref<TrialBase> aborted_trial = itsImpl->currentTrial();

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

void Block::processResponse(const Response& response)
{
DOTRACE("Block::processResponse");
  if (isComplete()) return;

  if (itsImpl->itsVerbose)
    {
      Util::log() << "response: " << response.val() << '\n';
    }

  DebugEval(response.correctVal());
  DebugEvalNL(response.val());

  if (!response.isCorrect())
    {
      // If the response was incorrect, add a repeat of the current
      // trial to the block and reshuffle
      addTrial(itsImpl->currentTrial(), 1);
      std::random_shuffle(
        itsImpl->itsTrialSequence.begin()+itsImpl->itsCurTrialSeqIdx+1,
        itsImpl->itsTrialSequence.end());
    }
}

void Block::endTrial()
{
DOTRACE("Block::endTrial");
  if (isComplete()) return;

  // Prepare to start next trial.
  ++itsImpl->itsCurTrialSeqIdx;

  DebugEval(numCompleted());
  DebugEval(numTrials());
  DebugEvalNL(isComplete());

  itsImpl->getExpt().edEndTrial();
}

void Block::nextTrial()
{
DOTRACE("Block::nextTrial");
  if ( !isComplete() )
    {
      beginTrial(itsImpl->getExpt());
    }
  else
    {
      itsImpl->getExpt().edNextBlock();
    }
}

void Block::haltExpt()
{
DOTRACE("Block::haltExpt");

  if ( itsImpl->itsHasBegun && !isComplete() )
    itsImpl->currentTrial()->trHaltExpt();
}

void Block::undoPrevTrial()
{
DOTRACE("Block::undoPrevTrial");

  DebugEval(itsImpl->itsCurTrialSeqIdx);

  // Check to make sure we've completed at least one trial
  if (itsImpl->itsCurTrialSeqIdx < 1) return;

  // Move the counter back to the previous trial...
  --itsImpl->itsCurTrialSeqIdx;

  // ...and erase the last response given to that trial
  if ( itsImpl->hasCurrentTrial() )
    {
      itsImpl->currentTrial()->undoLastResponse();
    }
}

void Block::resetBlock()
{
DOTRACE("Block::resetBlock");
  while (itsImpl->itsCurTrialSeqIdx > 0)
    {
      undoPrevTrial();
    }
}

static const char vcid_block_cc[] = "$Header$";
#endif // !BLOCK_CC_DEFINED
