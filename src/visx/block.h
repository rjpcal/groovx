///////////////////////////////////////////////////////////////////////
//
// block.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 12:29:33 1999
// written: Fri Jan 25 10:38:22 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_H_DEFINED
#define BLOCK_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRACER_H_DEFINED)
#include "util/tracer.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

namespace Util
{
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

class Block : public IO::IoObject
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

  /** Randomly permute the sequence of trialids, using seed as the
      random seed for the shuffle. */
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

  /** Returns the number of trials that have been successfully
      completed.  This number will not include trials that have been
      aborted either due to an invalid response or due to a
      timeout. */
  int numCompleted() const;

  /// Get the trial id (an index into Tlist) of the current trial.
  Util::SoftRef<TrialBase> currentTrial() const;

  /** Returns the integer type of the current trial (the result of
      TrialBase::trialType(). */
  int currentTrialType() const;

  /** Returns the last valid (but not necessarily "correct") response
      that was recorded in the current Block. */
  int prevResponse() const;

  /** Returns true if the current experiment is complete (i.e. all
      trials in the trial sequence have finished successfully), false
      otherwise. */
  bool isComplete() const;

  /** Returns a human(experimenter)-readable description of the
      current trial that shows the trial's id, the trial's type, the
      id's of the objects that are displayed in the trial, the
      categories of those objects, and the number of completed trials
      and number of total trials. */
  fstring trialDescription() const;

  /** Query whether the trialDescription() is printed to standard
      output at the beginning of each trial. */
  bool getVerbose() const;

  /** Change whether the trialDescription() is printed to standard
      output at the beginning of each trial. */
  void setVerbose(bool val);

  /////////////
  // actions //
  /////////////

  /** Does whatever is necessary at the beginning of a trial, and
      restarts the response time timer. */
  void beginTrial(Experiment& expt);

  /** Aborts the current trial of the experiment. The current (to be
      aborted) trial is put at the end of the trial sequence in the
      Block, without recording any response for that trial. The next
      call to drawTrial will start the same trial that would have
      started if the current trial had been completed normally,
      instead of being aborted. */
  void abortTrial();

  /** Record a response to the current trial in the Block, and prepare
      the Block for the next trial. The response is recorded for the
      current trial, and the Block's trial sequence index is
      incremented. Also, the next call to prevResponse will return the
      response that was recorded in the present command. */
  void processResponse(const Response& response);

  /** Prepares the Block to start the next trial by incrementing the
      trial sequence index. */
  void endTrial();

  /// Begins the next trial, moving on to the next Block if necessary.
  void nextTrial();

  /// Undraws, aborts, and ends the current trial.
  void haltExpt();

  /** The state of the experiment is restored to what it was just
      prior to the beginning of the most recent successfully completed
      trial. Thus, the current trial is changed to its previous value,
      and the response to the most recently successfully completed
      trial is erased. After a call to undoPrevTrial(), the next
      invocation of drawTrial() will redo the trial that was undone in
      the present command.*/
  void undoPrevTrial();

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
