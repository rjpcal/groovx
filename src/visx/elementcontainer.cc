///////////////////////////////////////////////////////////////////////
//
// elementcontainer.cc
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Dec  5 16:43:54 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef ELEMENTCONTAINER_CC_DEFINED
#define ELEMENTCONTAINER_CC_DEFINED

#include "visx/elementcontainer.h"

#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "util/error.h"
#include "util/iter.h"
#include "util/log.h"
#include "util/rand.h"
#include "util/ref.h"

#include <algorithm>
#include <vector>

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

using Util::Ref;

class ElementContainer::Impl
{
public:
  Impl() : elements(), randSeed(0), sequencePos(0) {}

  std::vector<Ref<Element> > elements;

  int randSeed;                 // Random seed used to create element sequence
  unsigned int sequencePos;     // Index of the current element
                                // Also functions as # of completed elements
};

///////////////////////////////////////////////////////////////////////
//
// ElementContainer member functions
//
///////////////////////////////////////////////////////////////////////

ElementContainer::ElementContainer() :
  rep(new Impl)
{
DOTRACE("ElementContainer::ElementContainer");
}

ElementContainer::~ElementContainer() throw()
{
DOTRACE("ElementContainer::~ElementContainer");
  delete rep;
}

void ElementContainer::readFrom(IO::Reader& reader)
{
DOTRACE("ElementContainer::readFrom");
  clearElements();

  IO::ReadUtils::readObjectSeq<Element>
    (reader, "trialSeq", std::back_inserter(rep->elements));

  reader.readValue("randSeed", rep->randSeed);
  reader.readValue("curTrialSeqdx", rep->sequencePos);
  if (rep->sequencePos > rep->elements.size())
    {
      throw Util::Error("ElementContainer");
    }
}

void ElementContainer::writeTo(IO::Writer& writer) const
{
DOTRACE("ElementContainer::writeTo");
  IO::WriteUtils::writeObjectSeq(writer, "trialSeq",
                                 rep->elements.begin(),
                                 rep->elements.end());

  writer.writeValue("randSeed", rep->randSeed);
  writer.writeValue("curTrialSeqdx", rep->sequencePos);
}

///////////////////////////////////////////////////////////////////////
//
// ElementContainer's Element base interface
//
///////////////////////////////////////////////////////////////////////

int ElementContainer::trialType() const
{
DOTRACE("ElementContainer::trialType");
  if (isComplete()) return -1;

  dbgEvalNL(3, currentElement()->trialType());

  return currentElement()->trialType();
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

fstring ElementContainer::vxInfo() const
{
DOTRACE("ElementContainer::vxInfo");
  if (isComplete()) return fstring("complete");

  fstring msg("current element ", currentElement()->uniqueName(),
              ", completed ", numCompleted(), " of ", numElements());

  return msg;
}

void ElementContainer::vxHalt() const
{
DOTRACE("ElementContainer::vxHalt");

  if ( !isComplete() )
    currentElement()->vxHalt();
}

void ElementContainer::vxReturn(ChildStatus s)
{
DOTRACE("ExptDriver::vxReturn");

  Precondition( !isComplete() );

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

  dbgEval(3, numCompleted());
  dbgEval(3, numElements());
  dbgEvalNL(3, isComplete());

  // Now, after (possibly) adjusting our sequence counter, see if we have
  // still have additional elements to run...
  if ( isComplete() )
    {
      vxAllChildrenFinished();
    }
  else
    {
      Util::log( vxInfo() );

      currentElement()->vxRun(*this);
    }
}

void ElementContainer::vxUndo()
{
DOTRACE("ElementContainer::vxUndo");
  dbgEval(3, rep->sequencePos);

  // FIXME how to know whether we should back up our own sequence, or
  // whether our child just backs up in its own sequence?

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
  vxHalt();

  Util::log("ElementContainer::vxReset");

  for (unsigned int i = 0; i < rep->elements.size(); ++i)
    {
      Util::log(fstring("resetting element", i));
      rep->elements[i]->vxReset();
    }

  rep->sequencePos = 0;
}


///////////////////////////////////////////////////////////////////////
//
// ElementContainer's container interface
//
///////////////////////////////////////////////////////////////////////

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

void ElementContainer::shuffle(int seed)
{
DOTRACE("ElementContainer::shuffle");

  setRandSeed(seed);

  Util::Urand generator(rep->randSeed);

  std::random_shuffle(rep->elements.begin(),
                      rep->elements.end(),
                      generator);
}

void ElementContainer::clearElements()
{
DOTRACE("ElementContainer::clearElements");
  vxHalt();

  rep->elements.clear();
  rep->sequencePos = 0;
}

Util::SoftRef<Element> ElementContainer::currentElement() const
{
DOTRACE("ElementContainer::currentElement");
  if (rep->sequencePos >= rep->elements.size())
    return Util::SoftRef<Element>();

  return rep->elements.at(rep->sequencePos);
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

Util::FwdIter<const Util::Ref<Element> > ElementContainer::getElements() const
{
DOTRACE("ElementContainer::getElements");

  return Util::FwdIter<const Util::Ref<Element> >
    (rep->elements.begin(), rep->elements.end());
}

bool ElementContainer::isComplete() const
{
DOTRACE("ElementContainer::isComplete");

  dbgEval(9, rep->sequencePos);
  dbgEvalNL(9, rep->elements.size());

  return (rep->sequencePos >= rep->elements.size());
}

static const char vcid_elementcontainer_cc[] = "$Header$";
#endif // !ELEMENTCONTAINER_CC_DEFINED
