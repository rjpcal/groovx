///////////////////////////////////////////////////////////////////////
//
// elementcontainer.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Dec  5 16:43:50 2002
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

#ifndef ELEMENTCONTAINER_H_DEFINED
#define ELEMENTCONTAINER_H_DEFINED

#include "visx/element.h"

namespace Util
{
  template <class T> class FwdIter;
  template <class T> class Ref;
  template <class T> class SoftRef;
}

template <class T> class minivec;

/// A base class for all composite visual experiment elements.
/** These might include "blocks" and "experiments". */
class ElementContainer: public Element
{
public:
  /// Default constructor.
  ElementContainer();

  /// Virtual destructor.
  virtual ~ElementContainer() throw();

  virtual void readFrom(IO::Reader* reader);

  virtual void writeTo(IO::Writer* writer) const;

  //
  // Element base interface
  //

  /// Returns the trial type of the current element.
  virtual int trialType() const;

  /// Returns the last valid response.
  /** But note that "valid" does not necessarily mean "correct". */
  virtual int lastResponse() const;

  /// Returns a human-readable description of the current element.
  /** This description includes the current element's id, its type, the
      id's of its subobjects, the categories of those objects, and the
      number of completed trials and number of total trials. */
  virtual fstring status() const;

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
  void addElement(Util::Ref<Element> element, unsigned int repeat = 1);

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
  Util::SoftRef<Element> currentElement() const;

  /// Returns the total number of child elements in the container.
  unsigned int numElements() const;

  /// Returns the number of elements that have been completed.
  /** This number will not include elements that have been aborted either
      due to an invalid response or due to a timeout. */
  unsigned int numCompleted() const;

  /// Returns an iterator to all the contained child elements.
  Util::FwdIter<const Util::Ref<Element> > getElements() const;

  /// Returns true if the all child elements are complete, false otherwise.
  bool isComplete() const;

protected:
  /// Hook function to be called when all children have been run.
  virtual void vxAllChildrenFinished() = 0;

  /// Throw an exception if there are not valid children still pending.
  void ensureNotComplete() const;

  /// Raw data access needed to support the legacy IO format.
  minivec<Util::Ref<Element> >& iolegacyElements() const;
  /// Raw data access needed to support the legacy IO format.
  int& iolegacyRandSeed() const;
  /// Raw data access needed to support the legacy IO format.
  unsigned int& iolegacySequencePos() const;

private:
  class Impl;
  Impl* const rep;
};

static const char vcid_elementcontainer_h[] = "$Header$";
#endif // !ELEMENTCONTAINER_H_DEFINED
