///////////////////////////////////////////////////////////////////////
//
// block.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 12:29:34 1999
// written: Thu Dec  5 16:34:01 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_CC_DEFINED
#define BLOCK_CC_DEFINED

#include "visx/block.h"

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

class ElementContainer
{
public:
  ElementContainer();

  ~ElementContainer();

  void readFrom(IO::Reader* reader);

  void writeTo(IO::Writer* writer);

  void addElement(Ref<Element> element, unsigned int repeat = 1);

  void setRandSeed(int s);
  int getRandSeed() const;

  void shuffle();

  void clearElements();

  Ref<Element> currentElement();

  int lastResponse() const;

  void childFinishedHelper(Element::ChildStatus s);

  void vxUndo();

  void vxReset();

  unsigned int numElements() const;

  unsigned int numCompleted() const;

  Util::FwdIter<Util::Ref<Element> > getElements();

  bool isComplete() const;

private:
  class Impl;
  Impl* const rep;
};

class ElementContainer::Impl
{
public:
  Impl() : elements(), randSeed(0), sequencePos(0) {}

  minivec<Ref<Element> > elements;

  int randSeed;                 // Random seed used to create element sequence
  unsigned int sequencePos;     // Index of the current element
                                // Also functions as # of completed elements
};

ElementContainer::ElementContainer() :
  rep(new Impl)
{
DOTRACE("ElementContainer::ElementContainer");
}

ElementContainer::~ElementContainer()
{
DOTRACE("ElementContainer::~ElementContainer");
  delete rep;
}

void ElementContainer::readFrom(IO::Reader* reader)
{
DOTRACE("ElementContainer::readFrom");
  clearElements();

  IO::ReadUtils::readObjectSeq<Element>
    (reader, "trialSeq", std::back_inserter(rep->elements));

  reader->readValue("randSeed", rep->randSeed);
  reader->readValue("curTrialSeqdx", rep->sequencePos);
  if (rep->sequencePos > rep->elements.size())
    {
      throw IO::ReadError("ElementContainer");
    }
}

void ElementContainer::writeTo(IO::Writer* writer)
{
DOTRACE("ElementContainer::writeTo");
  IO::WriteUtils::writeObjectSeq(writer, "trialSeq",
                                 rep->elements.begin(),
                                 rep->elements.end());

  writer->writeValue("randSeed", rep->randSeed);
  writer->writeValue("curTrialSeqdx", rep->sequencePos);
}

void ElementContainer::addElement(Ref<Element> element, unsigned int repeat)
{
DOTRACE("ElementContainer::addElement");

  for (unsigned int i = 0; i < repeat; ++i)
    {
      rep->elements.push_back(element);
    }
}

void ElementContainer::setRandSeed(int s)
{
DOTRACE("ElementContainer::setRandSeed");
  rep->randSeed = s;
}

int ElementContainer::getRandSeed() const
{
DOTRACE("ElementContainer::getRandSeed");
  return rep->randSeed;
}

void ElementContainer::shuffle()
{
DOTRACE("ElementContainer::shuffle");
  Util::Urand generator(rep->randSeed);

  std::random_shuffle(rep->elements.begin(),
                      rep->elements.end(),
                      generator);
}

void ElementContainer::clearElements()
{
DOTRACE("ElementContainer::clearElements");
  rep->elements.clear();
  rep->sequencePos = 0;
}

Ref<Element> ElementContainer::currentElement()
{
DOTRACE("ElementContainer::currentElement");
  Precondition(rep->sequencePos < rep->elements.size());

  return rep->elements.at(rep->sequencePos);
}

int ElementContainer::lastResponse() const
{
DOTRACE("ElementContainer::lastResponse");
  dbgEval(9, rep->sequencePos);
  dbgEvalNL(9, rep->elements.size());

  if (rep->sequencePos == 0 ||
      rep->elements.size() == 0) return -1;

  Ref<Element> prev_element = rep->elements.at(rep->sequencePos-1);

  return prev_element->lastResponse();
}

void ElementContainer::childFinishedHelper(Element::ChildStatus s)
{
DOTRACE("ElementContainer::childFinishedHelper");
  switch (s)
    {
    case Element::CHILD_OK:
      // Move on to the next element.
      ++rep->sequencePos;
      break;

    case Element::CHILD_REPEAT:
      {
        // Add a repeat of the current element to the sequence and reshuffle
        addElement(currentElement(), 1);
        std::random_shuffle
          (rep->elements.begin()+rep->sequencePos+1, rep->elements.end());

        // Move on to the next element.
        ++rep->sequencePos;
      }
      break;

    case Element::CHILD_ABORTED:
      {
        // Remember the element that we are about to abort so we can
        // store it at the end of the sequence.
        Ref<Element> aborted_element = currentElement();

        // Erase the aborted element from the sequence. Subsequent elements
        // will slide up to fill in the gap.
        rep->elements.erase(rep->elements.begin()+rep->sequencePos);

        // Add the aborted element to the back of the sequence.
        rep->elements.push_back(aborted_element);

        // Don't need to increment sequencePos here since the new trial
        // has "slid into place" by the reshuffling we've just done.
      }
      break;

    default:
      Assert(false);
    }
}

void ElementContainer::vxUndo()
{
DOTRACE("ElementContainer::vxUndo");
  dbgEval(3, rep->sequencePos);

  // Check to make sure we've completed at least one element
  if (rep->sequencePos < 1) return;

  // Move the counter back to the previous element...
  --rep->sequencePos;

  Assert(rep->sequencePos < rep->elements.size());

  // ...and undo that element
  currentElement()->vxUndo();
}

void ElementContainer::vxReset()
{
DOTRACE("ElementContainer::vxReset");
  for (unsigned int i = 0; i < rep->elements.size(); ++i)
    {
      rep->elements[i]->vxReset();
    }

  rep->sequencePos = 0;
}

unsigned int ElementContainer::numElements() const
{
DOTRACE("ElementContainer::numElements");
  return rep->elements.size();
}

unsigned int ElementContainer::numCompleted() const
{
DOTRACE("ElementContainer::numCompleted");
  return rep->sequencePos;
}

Util::FwdIter<Util::Ref<Element> > ElementContainer::getElements()
{
DOTRACE("ElementContainer::getElements");

  return Util::FwdIter<Util::Ref<Element> >
    (rep->elements.begin(), rep->elements.end());
}

bool ElementContainer::isComplete() const
{
DOTRACE("ElementContainer::isComplete");

  dbgEval(9, rep->sequencePos);
  dbgEvalNL(9, rep->elements.size());

  return (rep->sequencePos >= rep->elements.size());
}


class Block::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl() :
    parent(0)
  {}

  Element& getParent()
    {
      Precondition( parent != 0 );
      return *parent;
    }

  ElementContainer ec;

  Element* parent;
};

///////////////////////////////////////////////////////////////////////
//
// Block creators
//
///////////////////////////////////////////////////////////////////////

Block* Block::make()
{
DOTRACE("Block::make");
  return new Block;
}

Block::Block() :
  rep( new Impl )
{
DOTRACE("Block::Block");
}

Block::~Block()
{
  delete rep;
}

void Block::addElement(Ref<Element> element, int repeat)
{
DOTRACE("Block::addElement");

  rep->ec.addElement(element, repeat);
};

void Block::shuffle(int seed)
{
DOTRACE("Block::shuffle");
  rep->ec.setRandSeed(seed);
  rep->ec.shuffle();
}

void Block::clearAllElements()
{
DOTRACE("Block::clearAllElements");
  rep->ec.clearElements();
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

  rep->ec.readFrom(reader);

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

  rep->ec.writeTo(writer);
}

///////////////////////////////////////////////////////////////////////
//
// Block's Element interface
//
///////////////////////////////////////////////////////////////////////

Util::ErrorHandler& Block::getErrorHandler() const
{
DOTRACE("Block::getErrorHandler");
  return rep->getParent().getErrorHandler();
}

const Util::SoftRef<Toglet>& Block::getWidget() const
{
DOTRACE("Block::getWidget");
  return rep->getParent().getWidget();
}

int Block::trialType() const
{
DOTRACE("Block::trialType");
  if (isComplete()) return -1;

  dbgEvalNL(3, currentElement()->trialType());

  return currentElement()->trialType();
}

int Block::lastResponse() const
{
DOTRACE("Block::lastResponse");

  return rep->ec.lastResponse();
}

fstring Block::status() const
{
DOTRACE("Block::status");
  if (isComplete()) return fstring("block is complete");

  fstring msg;
  msg.append("next element ", currentElement().id(), ", ")
    .append(currentElement()->status())
    .append(", completed ", numCompleted(), " of ", numElements());

  return msg;
}

void Block::vxRun(Element& e)
{
DOTRACE("Block::vxRun");

  if ( isComplete() ) return;

  Precondition( &e != 0 );

  Util::log( status() );

  rep->parent = &e;

  currentElement()->vxRun(*this);
}

void Block::vxHalt() const
{
DOTRACE("Block::vxHalt");

  if ( !isComplete() )
    currentElement()->vxHalt();
}

void Block::vxEndTrialHook()
{
DOTRACE("Block::vxEndTrialHook");

  if (rep->parent != 0)
    rep->parent->vxEndTrialHook();
}

void Block::vxChildFinished(ChildStatus s)
{
DOTRACE("Block::vxChildFinished");
  Assert( !isComplete() );

  rep->ec.childFinishedHelper(s);

  dbgEval(3, numCompleted());
  dbgEval(3, numElements());
  dbgEvalNL(3, isComplete());

  // Now, after (possibly) adjusting our sequence counter, see if we have
  // still have additional elements to run...
  if ( !isComplete() )
    {
      Util::log( status() );

      currentElement()->vxRun(*this);
    }
  else
    {
      // Release our current parent, then pass control onto it.
      Element& p = rep->getParent();
      rep->parent = 0;
      p.vxChildFinished(CHILD_OK);
    }
}

void Block::vxUndo()
{
DOTRACE("Block::vxUndo");

  rep->ec.vxUndo();
}

void Block::vxReset()
{
DOTRACE("Block::vxReset");
  rep->ec.vxReset();
}

///////////////////////////////////////////////////////////////////////
//
// Block accessors
//
///////////////////////////////////////////////////////////////////////

int Block::numElements() const
{
DOTRACE("Block::numElements");
  return rep->ec.numElements();
}

int Block::numCompleted() const
{
DOTRACE("Block::numCompleted");
  return rep->ec.numCompleted();
}

Util::FwdIter<Util::Ref<Element> > Block::getElements() const
{
  return rep->ec.getElements();
}

Util::SoftRef<Element> Block::currentElement() const
{
DOTRACE("Block::currentElement");
  if (isComplete()) return Util::SoftRef<Element>();

  return rep->ec.currentElement();
}

bool Block::isComplete() const
{
DOTRACE("Block::isComplete");
  return rep->ec.isComplete();
}

static const char vcid_block_cc[] = "$Header$";
#endif // !BLOCK_CC_DEFINED
