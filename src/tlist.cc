///////////////////////////////////////////////////////////////////////
//
// tlist.cc
// Rob Peters
// created: Fri Mar 12 14:39:39 1999
// written: Thu Oct 21 13:48:35 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_CC_DEFINED
#define TLIST_CC_DEFINED

#include "tlist.h"

#include <iostream.h>
#include <strstream.h>
#include <string>
#include <GL/gl.h>

#include "trial.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope declarations and helper functions
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string ioTag = "Tlist";
}

///////////////////////////////////////////////////////////////////////
//
// Tlist member functions
//
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Tlist::Tlist() : 
  PtrList<Trial>(1), itsCurTrial(0), itsVisibility(false) 
{
DOTRACE("Tlist::Tlist");
}

Tlist Tlist::theInstance;

Tlist& Tlist::theTlist() {
DOTRACE("Tlist::theTlist");
  return theInstance;
}

Tlist::~Tlist() {
DOTRACE("Tlist::~Tlist");
  clear();
}

//---------------------------------------------------------------------
//
// Tlist::serialize
//
//---------------------------------------------------------------------

void Tlist::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Tlist::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  // Always serialize the PtrList base
  if (true || flag & BASES) { 
	 PtrList<Trial>::serialize(os, flag);
  }

  // itsCurTrial
  os << itsCurTrial << sep;
  // itsVisibility
  os << itsVisibility << sep;

  if (os.fail()) throw OutputError(ioTag);
}

//---------------------------------------------------------------------
//
// Tlist::deserialize
//
//---------------------------------------------------------------------

void Tlist::deserialize(istream &is, IOFlag flag) {
DOTRACE("Tlist::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  // Always deserialize its PtrList<Trial> base
  if (true || flag & BASES) {
	 PtrList<Trial>::deserialize(is, flag);
  }

  // itsCurTrial
  is >> itsCurTrial;  DebugEvalNL(itsCurTrial);

  // If itsCurTrial is not a valid id...
  if ( !isValidId(itsCurTrial) ) {
	 // ...then we throw an exception, as long as we don't have the
	 // special case where the current trial is zero because the list
	 // is empty
	 if ( itsCurTrial != 0 || count() != 0 ) {
		itsVisibility = false;
		throw IoValueError(ioTag);
	 }
  }

  // itsVisibility
  int vis;
  is >> vis;
  itsVisibility = bool(vis);    // can't read directly into the bool

  if (is.fail()) throw InputError(ioTag);
}

int Tlist::charCount() const {
  return (ioTag.length() + 1
			 + gCharCount<int>(itsCurTrial) + 1
			 + gCharCount<bool>(itsVisibility) + 1
			 + PtrList<Trial>::charCount()
			 + 5 ); // fudge factor 5
}

//---------------------------------------------------------------------
//
// Tlist::readFromObjidsOnly
//
//---------------------------------------------------------------------

int Tlist::readFromObjidsOnly(istream &is, int num_trials, int offset) {
DOTRACE("Tlist::readFromObjidsOnly");
  // Remove all trials and resize itsTrials to 0
  clear();

  // Determine whether we will read to the end of the input stream, or
  // whether we will read only num_trials lines from the stream,
  // according to the convention set in the header file.
  bool read_to_eof = (num_trials < 0);

  const int BUF_SIZE = 200;
  char line[BUF_SIZE];

  int trial = 0;
  while ( (read_to_eof || trial < num_trials) 
          && is.getline(line, BUF_SIZE) ) {
	 // Allow for whole-line comments beginning with '#'. If '#' is
	 // seen, skip this line and continue with the next line. The trial
	 // count is unaffected.
	 if (line[0] == '#')
		continue;
	 istrstream ist(line);
	 Trial* t = new Trial;
	 t->readFromObjidsOnly(ist, offset);
    int id = PtrList<Trial>::insert(t);
    ++trial;
  }                          
  if (is.bad()) throw InputError(ioTag);

  // Return the number of trials that were loaded.
  return trial;
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
  if (isValidId(trial)) itsCurTrial = trial;
}

void Tlist::clear() {
DOTRACE("Tlist::clear");
  PtrList<Trial>::clear();
  itsCurTrial = 0;
}

/////////////
// actions //
/////////////

void Tlist::redraw() const {
DOTRACE("Tlist::redraw");
  glClear(GL_COLOR_BUFFER_BIT);
  drawCurTrial();
  glFlush();
}

void Tlist::undraw() {
DOTRACE("Tlist::undraw");
  undrawCurTrial();
  glFlush();
}

void Tlist::clearscreen() {
DOTRACE("clearscreen");
  setVisible(0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}

void Tlist::drawTrial(int trial) {
DOTRACE("Tlist::drawTrial");
  setCurTrial(trial);
  setVisible(true);
  drawCurTrial();
  glFlush();
}

void Tlist::undrawTrial(int trial) {
DOTRACE("Tlist::undrawTrial");
  setCurTrial(trial);
  setVisible(false);
  undrawCurTrial();
  glFlush();
}

void Tlist::drawCurTrial() const {
DOTRACE("Tlist::drawCurTrial");

  DebugEval(itsCurTrial);
  DebugEval(capacity());
  DebugEvalNL((void *) getPtr(itsCurTrial));

  if (itsVisibility) {
    getCheckedPtr(itsCurTrial)->trDraw();
  }
}

void Tlist::undrawCurTrial() const {
DOTRACE("Tlist::undrawCurTrial");

  DebugEval(itsCurTrial);
  DebugEval(capacity());
  DebugEvalNL((void *) getPtr(itsCurTrial));

  getCheckedPtr(itsCurTrial)->trUndraw();
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<Trial>
//
///////////////////////////////////////////////////////////////////////

#include "ptrlist.cc"
template class PtrList<Trial>;

static const char vcid_tlist_cc[] = "$Header$";
#endif // !TLIST_CC_DEFINED
