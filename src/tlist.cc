///////////////////////////////////////////////////////////////////////
// tlist.cc
// Rob Peters
// created: Fri Mar 12 14:39:39 1999
// written: Tue Mar 16 19:24:24 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_CC_DEFINED
#define TLIST_CC_DEFINED

#include "tlist.h"

#include <iostream.h>
#include <strstream.h>
#include <string>
#include <typeinfo>
#include <GL/gl.h>

#include "trial.h"
#include "objlist.h"
#include "poslist.h"

#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// File scope declarations and helper functions
///////////////////////////////////////////////////////////////////////

namespace {
  const int RESIZE_CHUNK = 20;
  const int BUF_SIZE = 200;
}

///////////////////////////////////////////////////////////////////////
// Tlist member functions
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Tlist::~Tlist() {
DOTRACE("Tlist::~Tlist");
  clearAllTrials();
}

IOResult Tlist::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Tlist::serialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(Tlist).name() << sep; }

  // itsObjList
  itsObjList.serialize(os, flag);

  // itsPosList
  itsPosList.serialize(os, flag);

  // itsTrials: first we serialize the size of itsTrials to be able to
  // later restore its size in deserialize. However we must also
  // serialize the number of non-NULL pointers in itsTrials, because
  // we will only serialize the Trial*'s that are non-NULL. But in
  // order for deserialize to later know for long it should keep
  // reading into new Trial's, it must know ahead of time how many it
  // will see--this is exactly num_non_null.
  os << itsTrials.size() << sep;
  int num_null=0;
  // using this instead of bad STL count() implementation
  for (vector<Trial *>::const_iterator ii = itsTrials.begin(); 
       ii != itsTrials.end();
       ii++) {
    if (*ii == NULL) num_null++;
  }
  int num_non_null = itsTrials.size() - num_null;
  os << num_non_null << endl;
  int c = 0;
  for (int i = 0; i < itsTrials.size(); i++) {
    if (itsTrials[i] != NULL) {
      os << i << sep;
      itsTrials[i]->serialize(os, flag);
      c++;
    }
  }
#ifdef LOCAL_DEBUG
  DUMP_VAL1(c);
  DUMP_VAL2(num_non_null);
#endif
  Assert(c==num_non_null);      // make sure we have counted correctly

  // itsCurTrial
  os << itsCurTrial << sep;
  // itsVisibility
  os << itsVisibility << sep;
  return checkStream(os);
}

IOResult Tlist::deserialize(istream &is, IOFlag flag) {
DOTRACE("Tlist::deserialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }
  if (flag & IO::TYPENAME) {
    string name;
    is >> name;
    if (name != string(typeid(Tlist).name())) { return IO_ERROR; }
  }

  // itsObjList
  const_cast<ObjList &>(itsObjList).deserialize(is, flag);

  // itsPosList
  const_cast<PosList &>(itsPosList).deserialize(is, flag);

  // itsTrials
  clearAllTrials();
  int size, num_non_null;
  is >> size >> num_non_null;
  Assert( num_non_null <= size );
  itsTrials.resize(size, NULL);
  int trial;
  for (int i = 0; i < num_non_null; i++) {
    is >> trial;
    itsTrials[trial] = new Trial(is, flag, itsObjList, itsPosList);
  }
  
  // itsCurTrial
  is >> itsCurTrial;
  // itsVisibility
  int vis;
  is >> vis;
  itsVisibility = bool(vis);    // can't read directly into the bool
  return checkStream(is);
}

IOResult Tlist::readFromObjidsOnly(istream &is, int num_trials, int offset) {
DOTRACE("Tlist::readFromObjidsOnly");
  clearAllTrials();

  // Determine whether we will read to the end of the input stream, or
  // whether we will read only num_trials lines from the stream,
  // according to the convention set in the header file.
  bool read_to_eof = (num_trials < 0);

  char line[BUF_SIZE];

  int trial = 0;
  while ( (read_to_eof || trial < num_trials) 
          && is.getline(line, BUF_SIZE) ) {
    istrstream ist(line);
    itsTrials.push_back(new Trial(itsObjList, itsPosList));
    itsTrials.back()->readFromObjidsOnly(ist, offset);
    trial++;
  }                          

  return checkStream(is);
}

///////////////
// accessors //
///////////////

bool Tlist::isValidTrial(int trial) const {
DOTRACE("Tlist::isValidTrial");
  return ( trial >= 0 && 
           trial < itsTrials.size() && 
           itsTrials[trial] != NULL );
}

Trial* Tlist::getTrial(int trial) const {
DOTRACE("Tlist::getTrial");
  if (!isValidTrial(trial)) return NULL;
  return itsTrials[trial];
}

void Tlist::getValidTrials(vector<int>& vec) const {
DOTRACE("Tlist::getValidTrials");
  vec.clear();
  for (int i = 0; i < itsTrials.size(); i++) {
    if (isValidTrial(i)) vec.push_back(i);
  }
}

//////////////////
// manipulators //
//////////////////

void Tlist::setVisible(bool vis) {
DOTRACE("Tlist::setVisible");
  itsVisibility = vis;
}

void Tlist::setCurTrial(int trial) {
DOTRACE("Tlist::setCurTrial");
  if (isValidTrial(trial)) itsCurTrial = trial;
}

void Tlist::addToTrial(int trial, int objid, int posid) {
DOTRACE("Tlist::addToTrial");
  if ( trial < 0 ) return;
  if ( trial >= itsTrials.size() ) {
    itsTrials.resize(trial + RESIZE_CHUNK, NULL);
  }
  if (itsTrials[trial] == NULL) {
    itsTrials[trial] = new Trial(itsObjList, itsPosList);
  }
  itsTrials[trial]->add(objid, posid);
}

void Tlist::clearTrial(int trial) {
DOTRACE("Tlist::clearTrial");
  if (isValidTrial(trial)) {
    delete itsTrials[trial];
    itsTrials[trial] = NULL;
  }
}

void Tlist::clearAllTrials() {
DOTRACE("Tlist::clearAllTrials");
  for (int i = 0; i < itsTrials.size(); i++) {
    // calling clearTrial(i) rather than just delete itsTrials[i] is a
    // little bit slower since we call isValidTrial() each time, but
    // this makes more sense from a maintenance standpoint since this
    // way there is only one function that gets to delete a Trial
    clearTrial(i);
  }
}

/////////////
// actions //
/////////////

void Tlist::drawTrial(int trial) {
DOTRACE("Tlist::drawTrial");
  setCurTrial(trial);
  setVisible(true);
  glClear(GL_COLOR_BUFFER_BIT);
  drawCurTrial();
  glFlush();
}

void Tlist::drawCurTrial() const {
DOTRACE("Tlist::drawCurTrial");
#ifdef LOCAL_DEBUG
  DUMP_VAL1(itsCurTrial);
  DUMP_VAL1(itsTrials.size());
  DUMP_VAL2((void *) itsTrials[itsCurTrial]);
#endif
  if (itsVisibility) {
    itsTrials[itsCurTrial]->action();
  }
}

static const char vcid_tlist_cc[] = "$Header$";
#endif // !TLIST_CC_DEFINED
