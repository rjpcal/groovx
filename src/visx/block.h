///////////////////////////////////////////////////////////////////////
//
// block.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 12:29:33 1999
// written: Wed Dec  4 15:41:27 2002
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

class TrialBase;

class Experiment;

class Response;

class fstring;

///////////////////////////////////////////////////////////////////////
/**
 *
 * The Block class represents a block as a sequence of trials in an
 * experiment. It maintains a sequence of trial id's, used as indices
 * into the global singleton Tlist, as well as an index for the
 * current trial. Block is responsible for computing response times,
 * and recording the response information into the Trial's. The Block
 * is run by the ExptDriver by alternately calling drawTrial() and one
 * of processResponse() or abortTrial().
 *
 **/
///////////////////////////////////////////////////////////////////////

class Block : public Element, public IO::IoObject
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

  /// Add the specified trialid to the block, 'repeat' number of times.
  void addTrial(Util::Ref<TrialBase> trial, int repeat=1);

  /// Randomly permute the sequence of trialids.
  /** @param seed used as the random seed for the shuffle. */
  void shuffle(int seed=0);

  /// Clear the Block's list of trial ids.
  void removeAllTrials();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ///////////////
  // accessors //
  ///////////////

  /// Returns the total number of trials that will comprise the Block.
  int numTrials() const;

  /// Returns an iterator to all the trials contained in the Block.
  Util::FwdIter<Util::Ref<TrialBase> > trials() const;

  /// Returns the number of trials that have been successfully completed.
  /** This number will not include trials that have been aborted either due
      to an invalid response or due to a timeout. */
  int numCompleted() const;

  /// Get the trial id (an index into Tlist) of the current trial.
  Util::SoftRef<TrialBase> currentTrial() const;

  /// Returns the integer type of the current trial.
  /** (That is, the result of TrialBase::trialType(). */
  int currentTrialType() const;

  /// Returns the last valid response recorded in the Block.
  /** But note that "valid" does not necessarily mean "correct". */
  int prevResponse() const;

  /// Returns true if the block is complete, false otherwise.
  /** The block is considered complete if all trials in the trial sequence
      have finished successfully. */
  bool isComplete() const;

  /// Returns a human-readable description of the current trial.
  /** This description may include trial's id, the trial's type, the id's
      of the objects that are displayed in the trial, the categories of
      those objects, and the number of completed trials and number of total
      trials. */
  virtual fstring status() const;

  /////////////
  // actions //
  /////////////

  /// Starts the current trial.
  /** This will be called by nextTrial() as needed. */
  void beginTrial(Experiment& expt);

  /// Aborts the current trial of the experiment.
  /** The current (to be aborted) trial is put at the end of the trial
      sequence in the Block, without recording any response for that
      trial. The next call to drawTrial will start the same trial that
      would have started if the current trial had been completed normally,
      instead of being aborted. */
  void abortTrial();

  /// Records a response to the current trial in the Block.
  /** Also, prepares the Block for the next trial. The response is recorded
      for the current trial, and the Block's trial sequence index is
      incremented. Also, the next call to prevResponse will return the
      response that was recorded in the present command. */
  void processResponse(const Response& response);

  /// Prepares the Block to start the next trial. */
  void endTrial();

  /// Begins the next trial, moving on to the next Block if necessary.
  void nextTrial();

  /// Undraws, aborts, and ends the current trial.
  void haltExpt();

  /// Undo the previous trial.
  /** The state of the experiment is restored to what it was just prior to
      the beginning of the most recent successfully completed trial. Thus,
      the current trial is changed to its previous value, and the response
      to the most recently successfully completed trial is erased. After a
      call to undoPrevTrial(), the next invocation of drawTrial() will redo
      the trial that was undone in the present command.*/
  void undoPrevTrial();

  /// Restore the Block to a state where none of the trials have been run.
  /** Undoes all responses to all of the Trial's in the Block, and
      puts its trial counter back to the beginning of the block. */
  void resetBlock();

private:
  Block(const Block&);
  Block& operator=(const Block&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_block_h[] = "$Header$";
#endif // !BLOCK_H_DEFINED
