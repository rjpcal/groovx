///////////////////////////////////////////////////////////////////////
//
// block.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 12:29:34 1999
// written: Thu Jul  8 10:40:28 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCK_CC_DEFINED
#define BLOCK_CC_DEFINED

#include "block.h"

#include <iostream.h>
#include <strstream.h>
#include <string>
#include <sys/time.h>
#include <list>

#include "rand.h"
#include "iostl.h"
#include "tlist.h"
#include "trial.h"
#include "timeutils.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  Tlist& theTlist = Tlist::theTlist();
  const string ioTag = "Block";
}

///////////////////////////////////////////////////////////////////////
//
// Block member functions
//
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Block::Block() :
  itsTrialSequence(),
  itsRandSeed(0),
  itsCurTrialSeqIdx(0),
  itsVerbose(false)
{
DOTRACE("Block::Block");
}

void Block::addTrial(int trialid, int repeat) {
DOTRACE("Block::addTrial");
  for (int i = 0; i < repeat; ++i) {
	 itsTrialSequence.push_back(trialid);
  }
};

void Block::addTrials(int first_trial, int last_trial, int repeat) {
DOTRACE("Block::addTrials");

  if (first_trial > last_trial) return;

  list<int> ids;
  theTlist.insertValidIds(back_inserter(ids));

  if (first_trial != -1) {
	 remove_if(ids.begin(), ids.end(), bind2nd(less<int>(), first_trial));
  }
  if (last_trial != -1) {
	 remove_if(ids.begin(), ids.end(), bind2nd(greater<int>(), last_trial));
  }

  for (int i = 0; i < repeat; ++i) {
	 copy(ids.begin(), ids.end(), back_inserter(itsTrialSequence));
  }

#if 0
  Tlist::iterator start =
	 (first_trial == -1) ? theTlist.begin() : theTlist.at(first_trial);
  Tlist::iterator finish =
	 (last_trial == -1)  ? theTlist.end()   : theTlist.at(last_trial+1);

  while (start != finish) {
	 addTrial(start.toInt(), repeat);
	 ++start;
  }
#endif
}

void Block::shuffle(int seed) {
DOTRACE("Block::shuffle");
  itsRandSeed = seed;

  Urand generator(seed);
  
  random_shuffle(itsTrialSequence.begin(), itsTrialSequence.end(),
					  generator);
}

void Block::reset() {
DOTRACE("Block::reset");
  itsTrialSequence.clear();
  itsCurTrialSeqIdx = 0;
}

void Block::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Block::serialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  // itsTrialSequence
  serializeVecInt(os, itsTrialSequence);
  os << endl;
  // itsRandSeed
  os << itsRandSeed << endl;
  // itsCurTrialSeqIdx
  os << itsCurTrialSeqIdx << endl;
  // itsVerbose
  os << itsVerbose << endl;

  if (os.fail()) throw OutputError(ioTag);
}

void Block::deserialize(istream &is, IOFlag flag) {
DOTRACE("Block::deserialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }
  
  // itsTrialSequence
  deserializeVecInt(is, itsTrialSequence);

  // itsRandSeed
  is >> itsRandSeed;
  // itsCurTrialSeqIdx
  is >> itsCurTrialSeqIdx;
  if (itsCurTrialSeqIdx < 0 || itsCurTrialSeqIdx > itsTrialSequence.size()) {
	 throw IoValueError(ioTag);
  }
  // itsVerbose
  int val;
  is >> val;
  itsVerbose = bool(val);

  is.ignore(1, '\n');
  if (is.fail()) throw InputError(ioTag);
}

int Block::charCount() const {
  return (ioTag.size() + 1
			 + charCountVecInt(itsTrialSequence) + 1
			 + gCharCount<int>(itsRandSeed) + 1
			 + gCharCount<int>(itsCurTrialSeqIdx) + 1
			 + gCharCount<bool>(itsVerbose) + 1
			 + 5); //fudge factor
}

///////////////
// accessors //
///////////////

Trial& Block::getCurTrial() const {
DOTRACE("Block::getCurTrial");
  Assert(theTlist.isValidId(currentTrial()));
  return *(theTlist.getPtr(currentTrial()));
}

int Block::numTrials() const {
DOTRACE("Block::numTrials");
  return itsTrialSequence.size();
}

int Block::numCompleted() const {
DOTRACE("Block::numCompleted");
  return itsCurTrialSeqIdx;
}

int Block::currentTrial() const {
DOTRACE("Block::currentTrial");
  if (isComplete()) return -1;

  return itsTrialSequence[itsCurTrialSeqIdx];
}

int Block::currentTrialType() const {
DOTRACE("Block::currentTrialType");
  if (isComplete()) return -1;

  DebugEval(currentTrial());
  DebugEvalNL(getCurTrial().trialType());

  return getCurTrial().trialType();
}

int Block::prevResponse() const {
DOTRACE("Block::prevResponse");

#if 0
  DebugEval(itsCurTrialSeqIdx);
  DebugEvalNL(itsTrialSequence.size());
#endif

  if (itsCurTrialSeqIdx == 0 || itsTrialSequence.size() == 0) return -1;

  return theTlist.getCheckedPtr(
                       itsTrialSequence[itsCurTrialSeqIdx-1])->lastResponse();
}

bool Block::isComplete() const {
DOTRACE("Block::isComplete");

#if 0
  DebugEval(itsCurTrialSeqIdx);
  DebugEvalNL(itsTrialSequence.size());
#endif

  return (itsCurTrialSeqIdx >= itsTrialSequence.size());
}

const char* Block::trialDescription() const {
DOTRACE("Block::trialDescription");
  if (isComplete()) return "block is complete";

  const int BUF_SIZE = 200;
  static char buf[BUF_SIZE];	  // static because the address is returned

  ostrstream ost(buf, BUF_SIZE);
  ost << "trial id == " << currentTrial() << ", ";
  ost << getCurTrial().description();
  ost << ", completed " << numCompleted()
		<< " of " << numTrials();
  ost << '\0';

  return buf;
}

///////////////////////////////////////////////////////////////////////
//
// actions 
//
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
//
// Block::beginTrial --
//
// Do whatever is necessary at the beginning of a trial.
//
//--------------------------------------------------------------------

void Block::beginTrial() {
DOTRACE("Block::beginTrial");
  if (itsVerbose) {
	 cerr << trialDescription() << endl;
  }

  // Record when the trial began so we can later note the response
  // time.  We record the time *after* the drawTrial call since
  // rendering the trial can possibly take considerable time (order of
  // 100 - 1000ms).
  gettimeofday(&itsBeginTime, NULL);
}

//--------------------------------------------------------------------
//
// Block::drawTrial --
//
// The current trial is drawn, The current trial is not changed until
// a call either to processResponse, abortTrial, or endTrial; thus,
// multiple calls in a row to drawTrial will simply show the same
// trial repeatedly, although the response time timer will be
// restarted each time.
//
//--------------------------------------------------------------------

void Block::drawTrial() {
DOTRACE("Block::drawTrial");
  if (isComplete()) return;

  // This single call sets the Tlist's current trial and visibility
  // appropriately, then renders the objects and flushes the graphics.
  theTlist.drawTrial(currentTrial());
}

//--------------------------------------------------------------------
//
// Block::undrawTrial --
//
// The current trial is erased from the screen, and the Tlist's
// visibility is set to false, so that the trial does not reappear if
// any redraw events are sent to the screen window.
//
//--------------------------------------------------------------------

void Block::undrawTrial() {
DOTRACE("Block::undrawTrial");
  if (isComplete()) return;

  theTlist.undrawTrial(currentTrial());
}

//--------------------------------------------------------------------
//
// Block::abortTrial --
//
// Aborts the current trial of the experiment. The current (to be
// aborted) trial is put at the end of the trial sequence in the Block,
// without recording any response for that trial. The next call to
// drawTrial will start the same trial that would have started if the
// current trial had been completed normally, instead of being
// aborted.
//
//--------------------------------------------------------------------

void Block::abortTrial() {
DOTRACE("Block::abortTrial");
  if (isComplete()) return;

  // Remember the trial that we are about to abort so we can store it
  // at the end of the sequence.
  int aborted_trial = currentTrial();

  // Erase the aborted trial from the sequence. Subsequent trials will
  // slide up to fill in the gap.
  itsTrialSequence.erase(itsTrialSequence.begin()+itsCurTrialSeqIdx);

  // Add the aborted trial to the back of the sequence.
  itsTrialSequence.push_back(aborted_trial);

  // We must decrement itsCurTrialSeqIdx, so that when it is
  // incremented by endTrial, the next trial has slid into the
  // position where the aborted trial once was.
  --itsCurTrialSeqIdx;
}

//--------------------------------------------------------------------
//
// Block::processResponse --
//
// Record a response to the current trial in the Block, and prepare the
// Block for the next trial.
//
// Results:
// none.
//
// Side effects:
// The response is recorded for the current trial, and the Block's
// trial sequence index is incremented. In this way, the next call to 
// drawTrial will start the next trial. Also, the next call to 
// prevResponse will return the response that was recorded in the
// present command.
//
//--------------------------------------------------------------------

void Block::processResponse(int response) {
DOTRACE("Block::processResponse");
  if (isComplete()) return;

  int msec = elapsedMsecSince(itsBeginTime);

  getCurTrial().recordResponse(response, msec);

  if (itsVerbose) {
    cerr << "response " << response << endl;
  }
}

//---------------------------------------------------------------------
//
// Block::endTrial --
//
//---------------------------------------------------------------------

void Block::endTrial() {
DOTRACE("Block::endTrial");
  if (isComplete()) return;

  // XXX Right now this will cause a semantic problem (but not a
  // crash) if both processResponse and endTrial are called by a
  // high-level caller for the same Trial...
  processResponse(-1);

  // Prepare to start next trial.
  ++itsCurTrialSeqIdx;
}

//---------------------------------------------------------------------
//
// Block::haltExpt --
//
//---------------------------------------------------------------------

void Block::haltExpt() {
DOTRACE("Block::haltExpt");
  undrawTrial();
  abortTrial();
}

//--------------------------------------------------------------------
//
// Block::undoPrevTrial --
//
// This function undoes the last *successfully* completed trial. It
// moves the trial counter back one, then erases the most recent
// response given to that trial. After this, the next invocation of
// drawTrial() will redo the trial that was undone in the present
// command.  In sum, the state of the experiment is restored to what
// it was just prior to the beginning of the most recent successfully
// completed trial.
//
//--------------------------------------------------------------------

void Block::undoPrevTrial() {
DOTRACE("Block::undoPrevTrial");

  // Check to make sure we've completed at least one trial
  if (itsCurTrialSeqIdx < 1) return;
  
  // Move the counter back to the previous trial...
  --itsCurTrialSeqIdx;

  // ...and erase the last response given to that trial
  getCurTrial().undoLastResponse();
}

static const char vcid_block_cc[] = "$Header$";
#endif // !BLOCK_CC_DEFINED
