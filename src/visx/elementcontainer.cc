///////////////////////////////////////////////////////////////////////
//
// elementcontainer.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Dec  5 16:43:54 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_VISX_ELEMENTCONTAINER_CC_UTC20050626084017_DEFINED
#define GROOVX_VISX_ELEMENTCONTAINER_CC_UTC20050626084017_DEFINED

#include "visx/elementcontainer.h"

#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "nub/log.h"
#include "nub/ref.h"

#include "rutz/error.h"
#include "rutz/iter.h"
#include "rutz/rand.h"

#include <algorithm>
#include <vector>

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

using Nub::Ref;

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
GVX_TRACE("ElementContainer::ElementContainer");
}

ElementContainer::~ElementContainer() throw()
{
GVX_TRACE("ElementContainer::~ElementContainer");
  delete rep;
}

void ElementContainer::readFrom(IO::Reader& reader)
{
GVX_TRACE("ElementContainer::readFrom");
  clearElements();

  IO::ReadUtils::readObjectSeq<Element>
    (reader, "trialSeq", std::back_inserter(rep->elements));

  reader.readValue("randSeed", rep->randSeed);
  reader.readValue("curTrialSeqdx", rep->sequencePos);
  if (rep->sequencePos > rep->elements.size())
    {
      throw rutz::error("ElementContainer", SRC_POS);
    }
}

void ElementContainer::writeTo(IO::Writer& writer) const
{
GVX_TRACE("ElementContainer::writeTo");
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
GVX_TRACE("ElementContainer::trialType");
  if (isComplete()) return -1;

  dbg_eval_nl(3, currentElement()->trialType());

  return currentElement()->trialType();
}

int ElementContainer::lastResponse() const
{
GVX_TRACE("ElementContainer::lastResponse");
  dbg_eval(9, rep->sequencePos);
  dbg_eval_nl(9, rep->elements.size());

  if (rep->sequencePos == 0 ||
      rep->elements.size() == 0) return -1;

  Ref<Element> prev_element = rep->elements.at(rep->sequencePos-1);

  return prev_element->lastResponse();
}

rutz::fstring ElementContainer::vxInfo() const
{
GVX_TRACE("ElementContainer::vxInfo");
  if (isComplete()) return rutz::fstring("complete");

  rutz::fstring msg("current element ",
                    currentElement()->uniqueName(),
                    ", completed ", numCompleted(),
                    " of ", numElements());

  return msg;
}

void ElementContainer::vxHalt() const
{
GVX_TRACE("ElementContainer::vxHalt");

  if ( !isComplete() )
    currentElement()->vxHalt();
}

void ElementContainer::vxReturn(ChildStatus s)
{
GVX_TRACE("ExptDriver::vxReturn");

  GVX_PRECONDITION( !isComplete() );

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
      GVX_ASSERT(false);
    }

  dbg_eval(3, numCompleted());
  dbg_eval(3, numElements());
  dbg_eval_nl(3, isComplete());

  // Now, after (possibly) adjusting our sequence counter, see if we have
  // still have additional elements to run...
  if ( isComplete() )
    {
      vxAllChildrenFinished();
    }
  else
    {
      Nub::log( vxInfo() );

      currentElement()->vxRun(*this);
    }
}

void ElementContainer::vxUndo()
{
GVX_TRACE("ElementContainer::vxUndo");
  dbg_eval(3, rep->sequencePos);

  // FIXME how to know whether we should back up our own sequence, or
  // whether our child just backs up in its own sequence?

  // Check to make sure we've completed at least one element
  if (rep->sequencePos < 1) return;

  // Move the counter back to the previous element...
  --rep->sequencePos;

  GVX_ASSERT(rep->sequencePos < rep->elements.size());

  // ...and undo that element
  currentElement()->vxUndo();
}

void ElementContainer::vxReset()
{
GVX_TRACE("ElementContainer::vxReset");
  vxHalt();

  Nub::log("ElementContainer::vxReset");

  for (unsigned int i = 0; i < rep->elements.size(); ++i)
    {
      Nub::log(rutz::fstring("resetting element", i));
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
GVX_TRACE("ElementContainer::addElement");

  for (unsigned int i = 0; i < repeat; ++i)
    {
      rep->elements.push_back(element);
    }
}

void ElementContainer::setRandSeed(int s)
{
GVX_TRACE("ElementContainer::setRandSeed");
  rep->randSeed = s;
}

int ElementContainer::getRandSeed() const
{
GVX_TRACE("ElementContainer::getRandSeed");
  return rep->randSeed;
}

void ElementContainer::shuffle(int seed)
{
GVX_TRACE("ElementContainer::shuffle");

  setRandSeed(seed);

  rutz::urand generator(rep->randSeed);

  std::random_shuffle(rep->elements.begin(),
                      rep->elements.end(),
                      generator);
}

void ElementContainer::clearElements()
{
GVX_TRACE("ElementContainer::clearElements");
  vxHalt();

  rep->elements.clear();
  rep->sequencePos = 0;
}

Nub::SoftRef<Element> ElementContainer::currentElement() const
{
GVX_TRACE("ElementContainer::currentElement");
  if (rep->sequencePos >= rep->elements.size())
    return Nub::SoftRef<Element>();

  return rep->elements.at(rep->sequencePos);
}

unsigned int ElementContainer::numElements() const
{
GVX_TRACE("ElementContainer::numElements");
  return rep->elements.size();
}

unsigned int ElementContainer::numCompleted() const
{
GVX_TRACE("ElementContainer::numCompleted");
  return rep->sequencePos;
}

rutz::fwd_iter<const Nub::Ref<Element> >
ElementContainer::getElements() const
{
GVX_TRACE("ElementContainer::getElements");

  return rutz::fwd_iter<const Nub::Ref<Element> >
    (rep->elements.begin(), rep->elements.end());
}

bool ElementContainer::isComplete() const
{
GVX_TRACE("ElementContainer::isComplete");

  dbg_eval(9, rep->sequencePos);
  dbg_eval_nl(9, rep->elements.size());

  return (rep->sequencePos >= rep->elements.size());
}

static const char vcid_groovx_visx_elementcontainer_cc_utc20050626084017[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_ELEMENTCONTAINER_CC_UTC20050626084017_DEFINED
