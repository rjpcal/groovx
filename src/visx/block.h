///////////////////////////////////////////////////////////////////////
//
// block.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 12:29:33 1999
// written: Wed Dec  4 18:25:02 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_H_DEFINED
#define BLOCK_H_DEFINED

#include "io/io.h"

#include "util/tracer.h"

#include "visx/element.h"

namespace Util
{
  template <class T> class FwdIter;
  template <class T> class Ref;
  template <class T> class SoftRef;
}

class Response;

class fstring;

///////////////////////////////////////////////////////////////////////
/**
 *
 * The Block class represents a block as a sequence of elements in an
 * experiment. It maintains a sequence of elements, as well as an index for
 * the current element. Block is responsible for computing response times,
 * and recording the response information into the elements.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Block : public Element
{
public:
  /** This tracer dynamically controls the tracing of Block member
      functions. */
  static Util::Tracer tracer;

  //////////////
  // creators //
  //////////////

protected:
  /// Default constructor.
  Block();

public:
  /// Default creator.
  static Block* make();

  /// Virtual destructor ensures correct destruction of subclasses.
  virtual ~Block();

  /// Add the specified element to the block, 'repeat' number of times.
  void addElement(Util::Ref<Element> element, int repeat=1);

  /// Randomly permute the sequence of elements.
  /** @param seed used as the random seed for the shuffle. */
  void shuffle(int seed=0);

  /// Clear the Block's list of all its contained elements.
  void clearAllElements();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  //
  // Element interface
  //

  virtual Util::ErrorHandler& getErrorHandler() const;

  virtual const Util::SoftRef<Toglet>& getWidget() const;

  /// Returns the trial type of the current element.
  virtual int trialType() const;

  /// Returns a human-readable description of the current element.
  /** This description includes the current element's id, its type, the
      id's of its subobjects, the categories of those objects, and the
      number of completed trials and number of total trials. */
  virtual fstring status() const;

  /// Starts the current element.
  /** This will be called by vxNext() as needed. */
  void vxRun(Element& parent);

  /// Undraws, aborts, and ends the current element.
  virtual void vxHalt() const;

  /// Aborts the current element.
  /** The current (to be aborted) element is put at the end of the element
      sequence in the Block, without recording any response for that
      element. The next call to vxRun() will start the same element that
      would have started if the current element had been completed
      normally, instead of being aborted. */
  virtual void vxAbort();

  /// Undo the previous element.
  /** The state of the experiment is restored to what it was just prior to
      the beginning of the most recent successfully completed
      element. Thus, the current element is changed to its previous value,
      and the response to the most recently successfully completed element
      is erased. After a call to vxUndo(), the next invocation of
      vxRun() will redo the element that was undone in the present command.*/
  virtual void vxUndo();

  /// Prepares the Block to start the next element. */
  virtual void vxEndTrial();

  /// Begins the next element, moving on to the next Block if necessary.
  virtual void vxNext();

  /// Records a response to the current element in the Block.
  /** Also, prepares the Block for the next element. The response is
      recorded for the current element, and the Block's element sequence
      index is incremented. Also, the next call to lastResponse will return
      the response that was recorded in the present command. */
  virtual void vxProcessResponse(Response& response);

  ///////////////
  // accessors //
  ///////////////

  /// Returns the total number of elements that will comprise the Block.
  int numElements() const;

  /// Returns an iterator to all the elements contained in the Block.
  Util::FwdIter<Util::Ref<Element> > getElements() const;

  /// Returns the number of elements that have been successfully completed.
  /** This number will not include elements that have been aborted either
      due to an invalid response or due to a timeout. */
  int numCompleted() const;

  /// Get a reference to the current element.
  Util::SoftRef<Element> currentElement() const;

  /// Returns the last valid response recorded in the Block.
  /** But note that "valid" does not necessarily mean "correct". */
  int lastResponse() const;

  /// Returns true if the block is complete, false otherwise.
  /** The block is considered complete if all elements in the sequence
      have finished successfully. */
  bool isComplete() const;

  /////////////
  // actions //
  /////////////

  /// Restore the Block to a state where none of the elements have been run.
  /** Undoes all responses to all of the elements in the Block, and resets
      its counter back to the beginning. */
  void resetBlock();

private:
  Block(const Block&);
  Block& operator=(const Block&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_block_h[] = "$Header$";
#endif // !BLOCK_H_DEFINED
