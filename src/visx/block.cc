///////////////////////////////////////////////////////////////////////
//
// block.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 12:29:34 1999
// written: Sat Jun 26 12:30:40 1999
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

#include "rand.h"
#include "iostl.h"
#include "tlist.h"
#include "trial.h"

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
  const string ioTag = "Expt";
}

///////////////////////////////////////////////////////////////////////
//
// Expt member functions
//
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Expt::Expt() :
  itsTrialSequence(),
  itsRandSeed(0),
  itsCurTrialSeqIdx(0),
  itsVerbose(true)
{
DOTRACE("Expt::Expt");
}

void Expt::addTrials(int first_trial, int last_trial, int repeat) {
DOTRACE("Expt::addTrials");
  vector<int> id_vec;
  theTlist.getValidTrials(id_vec);

  // push all the trial id's that will be used in Expt onto a
  // vector, in order
  for (int i = 0; i < id_vec.size(); ++i) {
	 int id = id_vec[i];
	 if ( (id >= first_trial || first_trial == -1) &&
			(id <=  last_trial || last_trial  == -1) ) {
		for (int j = 0; j < repeat; ++j) {
		  itsTrialSequence.push_back(id);
		}
	 }
  }
}

void Expt::shuffle(int seed) {
DOTRACE("Expt::shuffle");
  itsRandSeed = seed;

  Urand generator(seed);
  
  random_shuffle(itsTrialSequence.begin(), itsTrialSequence.end(),
					  generator);
}

void Expt::reset() {
DOTRACE("Expt::reset");
  itsTrialSequence.clear();
  itsCurTrialSeqIdx = 0;
}

void Expt::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Expt::serialize");
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

void Expt::deserialize(istream &is, IOFlag flag) {
DOTRACE("Expt::deserialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, "TrialExpt " + ioTag); }
  
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

int Expt::charCount() const {
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

Trial& Expt::getCurTrial() const {
DOTRACE("Expt::getCurTrial");
  Assert(theTlist.isValidTrial(currentTrial()));
  return theTlist.getTrial(currentTrial());
}

int Expt::numTrials() const {
DOTRACE("Expt::numTrials");
  return itsTrialSequence.size();
}

int Expt::numCompleted() const {
DOTRACE("Expt::numCompleted");
  return itsCurTrialSeqIdx;
}

int Expt::currentTrial() const {
DOTRACE("Expt::currentTrial");
  if (isComplete()) return -1;

  return itsTrialSequence[itsCurTrialSeqIdx];
}

int Expt::currentTrialType() const {
DOTRACE("Expt::currentTrialType");
  if (isComplete()) return -1;

  DebugEval(currentTrial());
  DebugEvalNL(theTlist.getTrial(currentTrial()).trialType());

  return theTlist.getTrial(currentTrial()).trialType();
}

int Expt::prevResponse() const {
DOTRACE("Expt::prevResponse");

#if 0
  DebugEval(itsCurTrialSeqIdx);
  DebugEvalNL(itsTrialSequence.size());
#endif

  if (itsCurTrialSeqIdx == 0 || itsTrialSequence.size() == 0) return -1;

  return theTlist.getTrial(itsTrialSequence[itsCurTrialSeqIdx-1]).lastResponse();
}

bool Expt::isComplete() const {
DOTRACE("Expt::isComplete");

#if 0
  DebugEval(itsCurTrialSeqIdx);
  DebugEvalNL(itsTrialSequence.size());
#endif

  return (itsCurTrialSeqIdx >= itsTrialSequence.size());
}

const char* Expt::trialDescription() const {
DOTRACE("Expt::trialDescription");
  if (isComplete()) return "block is complete";

  const Trial& t = theTlist.getTrial(currentTrial());

  const int BUF_SIZE = 200;
  static char buf[BUF_SIZE];	  // static because the address is returned

  ostrstream ost(buf, BUF_SIZE);
  ost << "trial id == " << currentTrial() << ", ";
  ost << theTlist.getTrial(currentTrial()).description();
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
// Expt::beginTrial --
//
// Do whatever is necessary at the beginning of a trial.
//
//--------------------------------------------------------------------

void Expt::beginTrial() {
DOTRACE("Expt::beginTrial");
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
// Expt::drawTrial --
//
// The current trial is drawn, The current trial is not changed until
// a call either to processResponse, abortTrial, or endTrial; thus,
// multiple calls in a row to drawTrial will simply show the same
// trial repeatedly, although the response time timer will be
// restarted each time.
//
//--------------------------------------------------------------------

void Expt::drawTrial() {
DOTRACE("Expt::drawTrial");
  if (isComplete()) return;

  // This single call sets the Tlist's current trial and visibility
  // appropriately, then renders the objects and flushes the graphics.
  theTlist.drawTrial(currentTrial());
}

//--------------------------------------------------------------------
//
// Expt::undrawTrial --
//
// The current trial is erased from the screen, and the Tlist's
// visibility is set to false, so that the trial does not reappear if
// any redraw events are sent to the screen window.
//
//--------------------------------------------------------------------

void Expt::undrawTrial() {
DOTRACE("Expt::undrawTrial");
  if (isComplete()) return;

  theTlist.undrawTrial(currentTrial());
}

//--------------------------------------------------------------------
//
// Expt::abortTrial --
//
// Aborts the current trial of the experiment. The current (to be
// aborted) trial is put at the end of the trial sequence in the Expt,
// without recording any response for that trial. The next call to
// drawTrial will start the same trial that would have started if the
// current trial had been completed normally, instead of being
// aborted.
//
//--------------------------------------------------------------------

void Expt::abortTrial() {
DOTRACE("Expt::abortTrial");
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
// Expt::processResponse --
//
// Record a response to the current trial in the Expt, and prepare the
// Expt for the next trial.
//
// Results:
// none.
//
// Side effects:
// The response is recorded for the current trial, and the Expt's
// trial sequence index is incremented. In this way, the next call to 
// drawTrial will start the next trial. Also, the next call to 
// prevResponse will return the response that was recorded in the
// present command.
//
//--------------------------------------------------------------------

void Expt::processResponse(int response) {
DOTRACE("Expt::processResponse");
  if (isComplete()) return;

  static timeval endTime, elapsedTime;
  gettimeofday(&endTime, NULL);

  elapsedTime.tv_sec = endTime.tv_sec - itsBeginTime.tv_sec;
  elapsedTime.tv_usec = endTime.tv_usec - itsBeginTime.tv_usec;

  int msec = int(double(elapsedTime.tv_sec)*1000.0 +
					  double(elapsedTime.tv_usec)/1000.0);

  theTlist.getTrial(currentTrial()).recordResponse(response, msec);

  if (itsVerbose) {
    cerr << "response " << response << endl;
  }
}

//---------------------------------------------------------------------
//
// Expt::endTrial --
//
//---------------------------------------------------------------------

void Expt::endTrial() {
DOTRACE("Expt::endTrial");
  if (isComplete()) return;

  // XXX Right now this will cause a semantic problem (but not a
  // crash) if both processResponse and endTrial are called by a
  // high-level caller for the same Trial...
  processResponse(-1);

  // Prepare to start next trial.
  ++itsCurTrialSeqIdx;
}

//--------------------------------------------------------------------
//
// Expt::undoPrevTrial --
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

void Expt::undoPrevTrial() {
DOTRACE("Expt::undoPrevTrial");

  // Check to make sure we've completed at least one trial
  if (itsCurTrialSeqIdx < 1) return;
  
  // Move the counter back to the previous trial...
  --itsCurTrialSeqIdx;

  // ...and erase the last response given to that trial
  theTlist.getTrial(currentTrial()).undoLastResponse();
}

static const char vcid_block_cc[] = "$Header$";
#endif // !BLOCK_CC_DEFINED
