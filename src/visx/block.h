///////////////////////////////////////////////////////////////////////
//
// block.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 12:29:33 1999
// written: Wed Dec  1 15:26:26 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_H_DEFINED
#define BLOCK_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef STOPWATCH_H_DEFINED
#include "stopwatch.h"
#endif

class Trial;

///////////////////////////////////////////////////////////////////////
/**
 *
 * The Block class implements a block in an experiment. It maintains a
 * sequence of trial id's, used as indices into the global singleton
 * Tlist, as well as an index for the current trial. Block is
 * responsible for computing response times, and recording the
 * response information into the Trial's. The Block is run by the
 * ExptDriver by alternately calling drawTrial() and one of
 * processResponse() or abortTrial().
 *
 * @memo Represents a block as a sequence of trials in an experiment.
 **/
///////////////////////////////////////////////////////////////////////

class Block : public virtual IO {
public:
  //////////////
  // creators //
  //////////////

  ///
  Block();
  ///
  virtual ~Block() {}

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

  ///
  virtual void serialize(ostream &os, IOFlag flag) const;
  ///
  virtual void deserialize(istream &is, IOFlag flag);
  ///
  virtual int charCount() const;

  ///
  virtual void readFrom(Reader* reader);
  ///
  virtual void writeTo(Writer* writer) const;

  ///////////////
  // accessors //
  ///////////////
  
  ///
  Trial& getCurTrial() const;

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
      Trial::trialType(). */
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

  /** Manipulate whether the trialDescription() is printed to standard
      output at the beginning of each trial. */
  //@{
  ///
  virtual bool getVerbose() const { return itsVerbose; }
  ///
  virtual void setVerbose(bool val) { itsVerbose = val; }
  //@}

  /////////////
  // actions //
  /////////////

  /** Does whatever is necessary at the beginning of a trial, and
      restarts the response time timer. */
  virtual void beginTrial();

  /** The current trial is drawn. The index of the current trial is
		not changed until a call either to processResponse, abortTrial,
		or endTrial; thus, multiple calls in a row to drawTrial will
		simply show the same trial repeatedly. */
  virtual void drawTrial();

  /** The current trial is erased from the screen, and the Tlist's
		visibility is set to false, so that the trial does not reappear
		if any redraw events are sent to the screen window. */
  virtual void undrawTrial();

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
      incremented. In this way, the next call to drawTrial will start
      the next trial. Also, the next call to prevResponse will return
      the response that was recorded in the present command. */
  virtual void processResponse(int response);

  /** Prepares the Block to start the next trial by incrementing the
      trial sequence index. */
  virtual void endTrial();

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
  vector<int> itsTrialSequence; // Ordered sequence of indexes into the Tlist
										  // Also functions as # of completed trials

  int itsRandSeed;				  // Random seed used to create itsTrialSequence
  int itsCurTrialSeqIdx;		  // Index of the current trial
  bool itsVerbose;

  mutable StopWatch itsTimer;	  // Used to record the start time of each Trial
};

static const char vcid_block_h[] = "$Header$";
#endif // !BLOCK_H_DEFINED
