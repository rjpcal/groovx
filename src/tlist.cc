///////////////////////////////////////////////////////////////////////
// tlist.cc
// Rob Peters
// created: Fri Mar 12 14:39:39 1999
// written: Sat Mar 13 12:30:54 1999
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_CC_DEFINED
#define TLIST_CC_DEFINED

#include "tlist.h"

#include "trial.h"

// trials
bool Tlist::isValidTrial(int trial) const {
  return ( trial >= 0 && 
			  trial < itsTrials.size() && 
			  itsTrials[trial] != NULL );
}

void Tlist::drawTrial(int trial) {
  setCurTrial(trial);
  drawCurTrial();
}

void Tlist::drawCurTrial() const {
  itsTrials[itsCurTrial]->action();
}

void Tlist::setVisibile(bool vis) {
  itsVisibility = vis;
}

void Tlist::setCurTrial(int trial) {
  if (isValidTrial(trial)) itsCurTrial = trial;
}

void Tlist::addToTrial(int trial, int objid, int posid) {
  if ( trial < 0 || trial >= itsTrials.size() ) return;
  if (itsTrials[trial] == NULL) {
	 itsTrials[trial] = new Trial(itsObjList, itsPosList);
  }
  itsTrials[trial]->add(objid, posid);
}

static const char vcid_tlist_cc[] = "$Id$";
#endif // !TLIST_CC_DEFINED
