///////////////////////////////////////////////////////////////////////
//
// block.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 12:29:33 1999
// written: Fri Oct 20 17:27:57 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_H_DEFINED
#define BLOCK_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

class TrialBase;

class Experiment;

class Response;

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

class Block : public virtual IO::IoObject {
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
  void addTrial(int trialid, int repeat=1);

  /** Add to the Block all valid trialids between first_trial and
		last_trial, inclusive. If a boundary is set to -1, then the
		trialid is not checked against that boundary. Each trialid is
		added 'repeat' times. */
  void addTrials(int first_trial=-1, int last_trial=-1, int repeat=1);

  /** Randomly permute the sequence of trialids, using seed as the
		random seed for the shuffle. */
  void shuffle(int seed=0);

  /// Clear the Block's list of trial ids.
  void removeAllTrials();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ///////////////
  // accessors //
  ///////////////
  
  /// Returns a reference to the current \c Trial.
  TrialBase& getCurTrial() const;

  /// Returns the total number of trials that will comprise the Block.
  virtual int numTrials() const;

  /** Returns the number of trials that have been successfully
		completed.  This number will not include trials that have been
		aborted either due to an invalid response or due to a
		timeout. */
  virtual int numCompleted() const;

  /// Get the trial id (an index into Tlist) of the current trial.
  virtual int currentTrial() const;

  /** Returns the integer type of the current trial (the result of
      TrialBase::trialType(). */
  virtual int currentTrialType() const;

  /** Returns the last valid (but not necessarily "correct") response
		that was recorded in the current Block. */
  virtual int prevResponse() const;

  /** Returns true if the current experiment is complete (i.e. all
		trials in the trial sequence have finished successfully), false
		otherwise. */
  virtual bool isComplete() const;

  /** Returns a human(experimenter)-readable description of the
      current trial that shows the trial's id, the trial's type, the
      id's of the objects that are displayed in the trial, the
      categories of those objects, and the number of completed trials
      and number of total trials. */
  virtual const char* trialDescription() const;

  /** Query whether the trialDescription() is printed to standard
      output at the beginning of each trial. */
  virtual bool getVerbose() const;

  /** Change whether the trialDescription() is printed to standard
      output at the beginning of each trial. */
  virtual void setVerbose(bool val);

  /////////////
  // actions //
  /////////////

  /** Does whatever is necessary at the beginning of a trial, and
      restarts the response time timer. */
  virtual void beginTrial(Experiment& expt);

  /** The current trial is drawn. The index of the current trial is
		not changed until a call either to processResponse, abortTrial,
		or endTrial; thus, multiple calls in a row to drawTrial will
		simply show the same trial repeatedly. */
  virtual void drawTrialHook();

  /** Aborts the current trial of the experiment. The current (to be
      aborted) trial is put at the end of the trial sequence in the
      Block, without recording any response for that trial. The next
      call to drawTrial will start the same trial that would have
      started if the current trial had been completed normally,
      instead of being aborted. */
  virtual void abortTrial();

  /** Record a response to the current trial in the Block, and prepare
      the Block for the next trial. The response is recorded for the
      current trial, and the Block's trial sequence index is
      incremented. Also, the next call to prevResponse will return the
      response that was recorded in the present command. */
  virtual void processResponse(const Response& response);

  /** Prepares the Block to start the next trial by incrementing the
      trial sequence index. */
  virtual void endTrial();

  /// Begins the next trial, moving on to the next Block if necessary.
  virtual void nextTrial();

  /// Undraws, aborts, and ends the current trial.
  virtual void haltExpt();

  /** The state of the experiment is restored to what it was just
		prior to the beginning of the most recent successfully completed
		trial. Thus, the current trial is changed to its previous value,
		and the response to the most recently successfully completed
		trial is erased. After a call to undoPrevTrial(), the next
		invocation of drawTrial() will redo the trial that was undone in
		the present command.*/
  virtual void undoPrevTrial();

  /** Undoes all responses to all of the Trial's in the Block, and
		puts its trial counter back to the beginning of the block. */
  virtual void resetBlock();

private:
  Block(const Block&);
  Block& operator=(const Block&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_block_h[] = "$Header$";
#endif // !BLOCK_H_DEFINED
