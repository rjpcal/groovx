///////////////////////////////////////////////////////////////////////
// expt.cc
// Rob Peters
// created: Sat Mar 13 17:57:49 1999
// written: Wed May 19 14:35:44 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef EXPT_CC_DEFINED
#define EXPT_CC_DEFINED

#include "expt.h"

#include <iostream.h>
#include <strstream.h>
#include <string>
#include <typeinfo>
#include <sys/time.h>

#include "randutils.h"
#include "iostl.h"
#include "tlist.h"
#include "trial.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

// File scope data
namespace {
  const string ioTag = "Expt";
}

///////////////////////////////////////////////////////////////////////
// Expt member functions
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Expt::Expt(Tlist& tlist, int repeat, int seed, const string& date, 
			  const string& host, const string& subject) : 
  itsTlist(tlist)
{
DOTRACE("Expt::Expt");
  init(repeat, seed, date, host, subject);
}

void Expt::init(int repeat, int seed, const string& date, 
					 const string& host, const string& subject) {
DOTRACE("Expt::init");
  itsTrialSequence.clear();
  itsRandSeed = seed;
  itsCurTrialSeqIdx = 0;
  itsBeginDate = date;
  itsHostname = host;
  itsSubject = subject;

  itsAutosaveFile = "__autosave_file";

  itsUseFeedback = true;
  itsVerbose = true;

  itsAbortWait = 1000;
  itsAutosavePeriod = 10;
  itsInterTrialInterval = 1000;
  itsStimDur = 2000;
  itsTimeout = 4000;

  // obtain a list of all valid trial id's from itsTlist
  vector<int> id_vec;
  itsTlist.getValidTrials(id_vec);

  // push all the trial id's that will be used in Expt onto a
  // vector, in order
  vector<int> temp_vec;
  for (int i = 0; i < id_vec.size(); ++i) {
	 for (int j = 0; j < repeat; ++j) {
		temp_vec.push_back(id_vec[i]);
	 }
  }

  // pull the trial id's out of temp_vec in random order, pushing them
  // onto itsTrialSequence
  srand(seed);
  vector<int>::iterator itr;
  while ( !temp_vec.empty() ) {
	 itr = temp_vec.begin() + randIntRange(0, temp_vec.size());

#if 0
	 DebugEval(temp_vec.size());
	 DebugEvalNL(itr-temp_vec.begin());
#endif

	 itsTrialSequence.push_back(*itr);
	 temp_vec.erase(itr);
  }
}

void Expt::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Expt::serialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << typeid(Expt).name() << sep; }

  // itsTlist
  itsTlist.serialize(os, flag);

  // itsTrialSequence
  serializeVecInt(os, itsTrialSequence);
  os << endl;
  // itsRandSeed
  os << itsRandSeed << endl;
  // itsCurTrialSeqIdx
  os << itsCurTrialSeqIdx << endl;
  // itsBeginDate
  os << itsBeginDate << '\n';
  // itsHostname
  os << itsHostname << '\n';
  // itsSubject
  os << itsSubject << '\n';

  // itsEndDate;
  os << itsEndDate << '\n';

//   // itsAutosaveFile
//   os << itsAutosaveFile << '\n';
//   // itsKeyRespPairs
//   os << itsKeyRespPairs << '\n';
  
//   // itsUseFeedback
//   os << itsUseFeedback << ' ';
//   // itsVerbose
//   os << itsVerbose << ' ';
//   // itsAbortWait
//   os << itsAbortWait << ' ';
//   // itsAutosavePeriod
//   os << itsAutosavePeriod << ' ';
//   // itsInterTrialInterval
//   os << itsInterTrialInterval << ' ';
//   // itsStimDur
//   os << itsStimDur << ' ';
//   // itsTimeout
//   os << itsTimeout << ' ';

  if (os.fail()) throw OutputError(typeid(Expt));
}

void Expt::deserialize(istream &is, IOFlag flag) {
DOTRACE("Expt::deserialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) {
	 string name;
	 is >> name;
	 // Must also compare against "TrialExpt" for backward compatibility
	 if (name != typeid(Expt).name() && name != "TrialExpt") { 
		throw InputError(typeid(Expt));
	 }
  }
  
  // restore to default state
  init(0,0);

  // itsTlist
  itsTlist.deserialize(is, flag);

  // itsTrialSequence
  deserializeVecInt(is, itsTrialSequence);

  // itsRandSeed
  is >> itsRandSeed;
  // itsCurTrialSeqIdx
  is >> itsCurTrialSeqIdx;
  is.ignore(1, '\n');
  if (itsCurTrialSeqIdx < 0 || itsCurTrialSeqIdx > itsTrialSequence.size()) {
	 throw IoValueError(typeid(Expt));
  }
  // itsBeginDate
  if (is.peek() != EOF) { getline(is, itsBeginDate, '\n'); }
  // itsHostname
  if (is.peek() != EOF) { getline(is, itsHostname, '\n'); }
  // itsSubject
  if (is.peek() != EOF) { getline(is, itsSubject, '\n'); }

  // itsEndDate
  if (is.peek() != EOF) { getline(is, itsEndDate, '\n'); }

//   // itsAutosaveFile
//   if (is.peek() != EOF) { getline(is, itsAutosaveFile, '\n'); }
//   // itsKeyRespPairs
//   if (is.peek() != EOF) { getline(is, itsKeyRespPairs, '\n'); }

//   // itsUseFeedback
//   if (is.peek() != EOF) { int val; is >> val; itsUseFeedback = bool(val); }
//   // itsVerbose
//   if (is.peek() != EOF) { int val; is >> val; itsVerbose = bool(val); }
//   // itsAbortWait
//   if (is.peek() != EOF) { is >> itsAbortWait; }
//   // itsAutosavePeriod
//   if (is.peek() != EOF) { is >> itsAutosavePeriod; }
//   // itsInterTrialInterval
//   if (is.peek() != EOF) { is >> itsInterTrialInterval; }
//   // itsStimDur
//   if (is.peek() != EOF) { is >> itsStimDur; }
//   // itsTimeout
//   if (is.peek() != EOF) { is >> itsTimeout; }
  if (is.fail()) throw InputError(typeid(Expt));
}

int Expt::charCount() const {
  return (ioTag.size() + 1
			 + itsTlist.charCount() + 1
			 + charCountVecInt(itsTrialSequence) + 1
			 + charCountInt(itsRandSeed) + 1
			 + charCountInt(itsCurTrialSeqIdx) + 1
			 + itsBeginDate.size() + 1
			 + itsHostname.size() + 1
			 + itsSubject.size() + 1
			 + 5); //fudge factor
}

///////////////
// accessors //
///////////////

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
  DebugEvalNL(itsTlist.getTrial(currentTrial()).trialType());

  return itsTlist.getTrial(currentTrial()).trialType();
}

int Expt::prevResponse() const {
DOTRACE("Expt::prevResponse");

#if 0
  DebugEval(itsCurTrialSeqIdx);
  DebugEvalNL(itsTrialSequence.size());
#endif

  if (itsCurTrialSeqIdx == 0 || itsTrialSequence.size() == 0) return -1;

  return itsTlist.getTrial(itsTrialSequence[itsCurTrialSeqIdx-1]).lastResponse();
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
  if (isComplete()) return "expt is complete";

  const Trial& t = itsTlist.getTrial(currentTrial());

  const int BUF_SIZE = 200;
  static char buf[BUF_SIZE];	  // static because the address is returned

  ostrstream ost(buf, BUF_SIZE);
  ost << "trial id == " << currentTrial() << ", ";
  ost << itsTlist.getTrial(currentTrial()).description();
  ost << ", completed " << numCompleted()
		<< " of " << numTrials();
  ost << '\0';

  return buf;
}

// An autosave is requested only if itsAutosavePeriod is positive, and
// the number of completed trials is evenly divisible by the autosave
// period.
bool Expt::needAutosave() const {
DOTRACE("Expt::needAutosave");
  return ( (itsAutosavePeriod > 0) &&
			  ((numCompleted() % itsAutosavePeriod) == 0) );
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
// Start the next trial in the experiment.
//
// Results: 
// none.
//
// Side effects:
// Begin the next trial in the Expt. The trial is drawn, and
// the timer to measure response time is started. The current trial is 
// not changed until a call either to recordResponse or to abortTrial;
// thus, multiple calls in a row to beginTrial will simply show the 
// same trial repeatedly, although the response time timer will be
// restarted each time.
//
//--------------------------------------------------------------------

void Expt::beginTrial() {
DOTRACE("Expt::beginTrial");
  if (isComplete()) return;

  // This single call sets the Tlist's current trial and visibility
  // appropriately, then renders the objects and flushes the graphics.
  itsTlist.drawTrial(currentTrial());

  // Record when the trial began so we can later note the response
  // time.  We record the time *after* the drawTrial call since
  // rendering the trial can possibly take considerable time (order of
  // 100 - 1000ms).
  gettimeofday(&itsBeginTime, NULL);
}

//--------------------------------------------------------------------
//
// Expt::undrawTrial --
//
// Erase the current trial from the screen.
//
// Results:
// none
//
// Side effects:
// The current trial is erased from the screen, and the Tlist's
// visibility is set to false, so that the trial does not reappear if
// any redraw events are sent to the screen window.
//
//--------------------------------------------------------------------

void Expt::undrawTrial() {
DOTRACE("Expt::undrawTrial");
  if (isComplete()) return;

  itsTlist.undrawTrial(currentTrial());
}

//--------------------------------------------------------------------
//
// Expt::abortTrial --
//
// Abort the current trial of the experiment.
//
// Results:
// none.
//
// Side effects:
// The current (to be aborted) trial is put at the end of the trial
// sequence in the Expt, without recording any response for that
// trial. The next call to beginTrial will start the same trial that
// would have started if the current trial had been completed
// normally, instead of begin aborted.
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

  // itsCurTrialSeqIdx does not have to change, since the next trial
  // has slid into the position where the aborted trial once was.
}

//--------------------------------------------------------------------
//
// Expt::recordResponse --
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
// beginTrial will start the next trial. Also, the next call to 
// prevResponse will return the response that was recorded in the
// present command.
//
//--------------------------------------------------------------------

void Expt::recordResponse(int val) {
DOTRACE("Expt::recordResponse");
  if (isComplete()) return;

  static timeval endTime, elapsedTime;
  gettimeofday(&endTime, NULL);

  elapsedTime.tv_sec = endTime.tv_sec - itsBeginTime.tv_sec;
  elapsedTime.tv_usec = endTime.tv_usec - itsBeginTime.tv_usec;

  int msec = int(double(elapsedTime.tv_sec)*1000.0 +
					  double(elapsedTime.tv_usec)/1000.0);

  itsTlist.getTrial(currentTrial()).recordResponse(val, msec);

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
// beginTrial() will redo the trial that was undone in the present
// command.  In sum, the state of the experiment is restored to what
// it was just prior to the beginning of the most recent successfully
// completed trial.
//
// Results:
// none.
//
// Side effects:
// The most recent response of the previous trial is erased.
//
//--------------------------------------------------------------------

void Expt::undoPrevTrial() {
DOTRACE("Expt::undoPrevTrial");

  // Check to make sure we've completed at least one trial
  if (itsCurTrialSeqIdx < 1) return;
  
  // Move the counter back to the previous trial...
  --itsCurTrialSeqIdx;

  // ...and erase the last response given to that trial
  itsTlist.getTrial(currentTrial()).undoLastResponse();
}

static const char vcid_expt_cc[] = "$Header$";
#endif // !EXPT_CC_DEFINED
