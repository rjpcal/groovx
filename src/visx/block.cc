///////////////////////////////////////////////////////////////////////
//
// block.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 12:29:34 1999
// written: Wed Dec  4 17:33:44 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_CC_DEFINED
#define BLOCK_CC_DEFINED

#include "visx/block.h"

#include "visx/experiment.h"
#include "visx/response.h"

#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "util/iter.h"
#include "util/log.h"
#include "util/minivec.h"
#include "util/rand.h"
#include "util/ref.h"
#include "util/strings.h"

#include <algorithm>

#define DYNAMIC_TRACE_EXPR Block::tracer.status()
#include "util/trace.h"
#include "util/debug.h"

Util::Tracer Block::tracer;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  IO::VersionId BLOCK_SERIAL_VERSION_ID = 2;
}

///////////////////////////////////////////////////////////////////////
//
// Block::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Block::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl() :
    itsElements(),
    itsRandSeed(0),
    itsSequencePos(0),
    itsHasBegun(false),
    itsExperiment(0)
    {}

  minivec<Ref<Element> > itsElements;

  int itsRandSeed;              // Random seed used to create itsElements
  int itsSequencePos;           // Index of the current element
                                // Also functions as # of completed elements

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

  Ref<Element> currentElement()
    {
      Precondition(hasCurrentElement());

      return itsElements.at(itsSequencePos);
    }

  bool hasCurrentElement()
    {
      if ( itsSequencePos < 0 ||
           (unsigned int) itsSequencePos >= itsElements.size() )
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

void Block::addElement(Ref<Element> element, int repeat)
{
DOTRACE("Block::addElement");
  for (int i = 0; i < repeat; ++i)
    {
      itsImpl->itsElements.push_back(element);
    }
};

void Block::shuffle(int seed)
{
DOTRACE("Block::shuffle");
  itsImpl->itsRandSeed = seed;

  Util::Urand generator(seed);

  std::random_shuffle(itsImpl->itsElements.begin(),
                      itsImpl->itsElements.end(),
                      generator);
}

void Block::clearAllElements()
{
DOTRACE("Block::clearAllElements");
  itsImpl->itsElements.clear();
  itsImpl->itsSequencePos = 0;
}

IO::VersionId Block::serialVersionId() const
{
DOTRACE("Block::serialVersionId");
  return BLOCK_SERIAL_VERSION_ID;
}

void Block::readFrom(IO::Reader* reader)
{
DOTRACE("Block::readFrom");

  int svid = reader->ensureReadVersionId("Block", 1, "Try grsh0.8a3");

  itsImpl->itsElements.clear();
  IO::ReadUtils::readObjectSeq<Element>(
        reader, "trialSeq", std::back_inserter(itsImpl->itsElements));

  reader->readValue("randSeed", itsImpl->itsRandSeed);
  reader->readValue("curTrialSeqdx", itsImpl->itsSequencePos);
  if (itsImpl->itsSequencePos < 0 ||
      size_t(itsImpl->itsSequencePos) > itsImpl->itsElements.size())
    {
      throw IO::ReadError("Block");
    }

  if (svid < 2)
    {
      bool dummy;
      reader->readValue("verbose", dummy);
    }
}

void Block::writeTo(IO::Writer* writer) const
{
DOTRACE("Block::writeTo");

  writer->ensureWriteVersionId("Block", BLOCK_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a7");

  IO::WriteUtils::writeObjectSeq(writer, "trialSeq",
                                 itsImpl->itsElements.begin(),
                                 itsImpl->itsElements.end());

  writer->writeValue("randSeed", itsImpl->itsRandSeed);
  writer->writeValue("curTrialSeqdx", itsImpl->itsSequencePos);
}

///////////////
// accessors //
///////////////

int Block::numElements() const
{
DOTRACE("Block::numElements");
  return itsImpl->itsElements.size();
}

Util::FwdIter<Util::Ref<Element> > Block::getElements() const
{
  return Util::FwdIter<Util::Ref<Element> >
    (itsImpl->itsElements.begin(),
     itsImpl->itsElements.end());
}

int Block::numCompleted() const
{
DOTRACE("Block::numCompleted");
  return itsImpl->itsSequencePos;
}

Util::SoftRef<Element> Block::currentElement() const
{
DOTRACE("Block::currentElement");
  if (isComplete()) return Util::SoftRef<Element>();

  return itsImpl->currentElement();
}

int Block::trialType() const
{
DOTRACE("Block::trialType");
  if (isComplete()) return -1;

  dbgEvalNL(3, itsImpl->currentElement()->trialType());

  return itsImpl->currentElement()->trialType();
}

int Block::lastResponse() const
{
DOTRACE("Block::lastResponse");

  dbgEval(9, itsImpl->itsSequencePos);
  dbgEvalNL(9, itsImpl->itsElements.size());

  if (itsImpl->itsSequencePos == 0 ||
      itsImpl->itsElements.size() == 0) return -1;

  Ref<Element> prev_element =
    itsImpl->itsElements.at(itsImpl->itsSequencePos-1);
  return prev_element->lastResponse();
}

bool Block::isComplete() const
{
DOTRACE("Block::isComplete");

  dbgEval(9, itsImpl->itsSequencePos);
  dbgEvalNL(9, itsImpl->itsElements.size());

  // This is 'tricky'. The problem is that itsImpl->itsSequencePos may
  // temporarily be negative in between a vxAbort and the corresponding
  // vxEndTrial. This means that we can't only compare
  // itsImpl->itsSequencePos with itsImpl->itsElements.size(), because the
  // former is signed and the latter is unsigned, forcing a 'promotion' to
  // unsigned of the former... this makes it a huge positive number if it
  // was actually negative. Thus, we must also check that
  // itsImpl->itsSequencePos is actually non-negative before returning
  // 'true' from this function.
  return ((itsImpl->itsSequencePos >= 0) &&
          (size_t(itsImpl->itsSequencePos) >= itsImpl->itsElements.size()));
}

fstring Block::status() const
{
DOTRACE("Block::status");
  if (isComplete()) return fstring("block is complete");

  fstring msg;
  msg.append("next element ", currentElement().id(), ", ")
    .append(itsImpl->currentElement()->status())
    .append(", completed ", numCompleted(), " of ", numElements());

  return msg;
}

///////////////////////////////////////////////////////////////////////
//
// actions
//
///////////////////////////////////////////////////////////////////////

void Block::vxRun(Experiment& expt)
{
DOTRACE("Block::vxRun");

  if ( isComplete() ) return;

  itsImpl->itsHasBegun = true;

  Util::log( status() );

  itsImpl->setExpt(expt);

  itsImpl->currentElement()->
             vxRun(expt.getWidget(), expt.getErrorHandler(), *this);
}

void Block::vxAbort()
{
DOTRACE("Block::vxAbort");
  if (isComplete()) return;

  // Remember the element that we are about to abort so we can store it
  // at the end of the sequence.
  Ref<Element> aborted_element = itsImpl->currentElement();

  // Erase the aborted element from the sequence. Subsequent elements will
  // slide up to fill in the gap.
  itsImpl->itsElements.erase(
              itsImpl->itsElements.begin()+itsImpl->itsSequencePos);

  // Add the aborted element to the back of the sequence.
  itsImpl->itsElements.push_back(aborted_element);

  // We must decrement itsImpl->itsSequencePos, so that when it is
  // incremented by vxEndTrial, the next element has slid into the
  // position where the aborted element once was.
  --itsImpl->itsSequencePos;
}

void Block::processResponse(const Response& response)
{
DOTRACE("Block::processResponse");
  if (isComplete()) return;

  dbgEval(3, response.correctVal());
  dbgEvalNL(3, response.val());

  if (!response.isCorrect())
    {
      // If the response was incorrect, add a repeat of the current
      // element to the block and reshuffle
      addElement(itsImpl->currentElement(), 1);
      std::random_shuffle(
        itsImpl->itsElements.begin()+itsImpl->itsSequencePos+1,
        itsImpl->itsElements.end());
    }
}

void Block::vxEndTrial()
{
DOTRACE("Block::vxEndTrial");
  if (isComplete()) return;

  // Prepare to start next element.
  ++itsImpl->itsSequencePos;

  dbgEval(3, numCompleted());
  dbgEval(3, numElements());
  dbgEvalNL(3, isComplete());

  itsImpl->getExpt().edEndTrial();
}

void Block::vxNext()
{
DOTRACE("Block::vxNext");
  if ( !isComplete() )
    {
      vxRun(itsImpl->getExpt());
    }
  else
    {
      itsImpl->getExpt().edNextBlock();
    }
}

void Block::vxHalt()
{
DOTRACE("Block::vxHalt");

  if ( itsImpl->itsHasBegun && !isComplete() )
    itsImpl->currentElement()->vxHalt();
}

void Block::vxUndo()
{
DOTRACE("Block::vxUndo");

  dbgEval(3, itsImpl->itsSequencePos);

  // Check to make sure we've completed at least one element
  if (itsImpl->itsSequencePos < 1) return;

  // Move the counter back to the previous element...
  --itsImpl->itsSequencePos;

  // ...and erase the last response given to that element
  if ( itsImpl->hasCurrentElement() )
    {
      itsImpl->currentElement()->vxUndo();
    }
}

void Block::resetBlock()
{
DOTRACE("Block::resetBlock");
  while (itsImpl->itsSequencePos > 0)
    {
      vxUndo();
    }
}

static const char vcid_block_cc[] = "$Header$";
#endif // !BLOCK_CC_DEFINED
