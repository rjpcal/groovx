/** @file visx/elementcontainer.h abstract class for composite
    elements within a psychophysics experiment */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Dec  5 16:43:50 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_ELEMENTCONTAINER_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_ELEMENTCONTAINER_H_UTC20050626084016_DEFINED

#include "visx/element.h"

namespace rutz
{
  template <class T> class fwd_iter;
}

namespace nub
{
  template <class T> class ref;
  template <class T> class soft_ref;
}

/// A base class for all composite visual experiment elements.
/** These might include "blocks" and "experiments". */
class ElementContainer: public Element
{
public:
  /// Default constructor.
  ElementContainer();

  /// Virtual destructor.
  virtual ~ElementContainer() noexcept;

  virtual void read_from(io::reader& reader);

  virtual void write_to(io::writer& writer) const;

  //
  // Element base interface
  //

  /// Returns the trial type of the current element.
  virtual int trialType() const;

  /// Returns the last valid response.
  /** But note that "valid" does not necessarily mean "correct". */
  virtual int lastResponse() const;

  /// Returns a string naming the current element, plus a count of completed elements.
  virtual rutz::fstring vxInfo() const;

  /// Halt the current child element.
  virtual void vxHalt() const;

  /// Delegates partially on to vxAllChildrenFinished().
  virtual void vxReturn(ChildStatus s);

  /// Undo the previous element.
  /** The state of the experiment is restored to what it was just prior to
      the beginning of the most recent successfully completed
      element. Thus, the current element is changed to its previous value,
      and the response to the most recently successfully completed element
      is erased. After a call to vxUndo(), the next invocation of
      vxRun() will redo the element that was undone in the present command.*/
  virtual void vxUndo();

  /// Reset all of the contained child elements.
  virtual void vxReset();

  //
  // Container interface
  //

  /// Add the specified element to the sequence, 'repeat' number of times.
  void addElement(nub::ref<Element> element, unsigned int repeat = 1);

  /// Set the random seed for shuffling child elements.
  void setRandSeed(int s);

  /// Get the current random seed used for shuffling child elements.
  int getRandSeed() const;

  /// Randomly permute the sequence of elements.
  /** @param seed used as the random seed for the shuffle. */
  void shuffle(int seed=0);

  /// Clear the container of all its contained elements.
  void clearElements();

  /// Get a reference to the current element.
  nub::soft_ref<Element> currentElement() const;

  /// Returns the total number of child elements in the container.
  unsigned int numElements() const;

  /// Returns the number of elements that have been completed.
  /** This number will not include elements that have been aborted either
      due to an invalid response or due to a timeout. */
  unsigned int numCompleted() const;

  /// Returns an iterator to all the contained child elements.
  rutz::fwd_iter<const nub::ref<Element> > getElements() const;

  /// Returns true if the all child elements are complete, false otherwise.
  bool isComplete() const;

protected:
  /// Hook function to be called when all children have been run.
  virtual void vxAllChildrenFinished() = 0;

private:
  class Impl;
  Impl* const rep;
};

#endif // !GROOVX_VISX_ELEMENTCONTAINER_H_UTC20050626084016_DEFINED
