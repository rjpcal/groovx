///////////////////////////////////////////////////////////////////////
//
// tlistwidget.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Dec  3 14:46:38 1999
// written: Wed Mar  8 11:07:06 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTWIDGET_CC_DEFINED
#define TLISTWIDGET_CC_DEFINED

#include "tlistwidget.h"

#include "canvas.h"
#include "tlist.h"
#include "trial.h"

#include "trace.h"
#include "debug.h"


namespace {
  Tlist& theTlist = Tlist::theTlist();
}


TlistWidget::TlistWidget(Togl* togl, double dist, double unit_angle) :
  ToglConfig(togl, dist, unit_angle),
  itsCurTrial(0),
  itsVisibility(false)
{
DOTRACE("TlistWidget::TlistWidget");
}

void TlistWidget::display() {
DOTRACE("TlistWidget::display");
  // First we must erase the previous current trial. We ignore any
  // invalid id errors that occur, and simply clear the screen in
  // this case.
  try {
	 theTlist.getCheckedPtr(itsCurTrial)->trUndraw(*(this->getCanvas()), false);
  }
  catch (InvalidIdError&) {
	 clearscreen();
  }

  safeDrawTrial();
  swapBuffers();
  getCanvas()->flushOutput();
}

void TlistWidget::clearscreen() {
DOTRACE("TlistWidget::clearscreen");
  setVisibility(false);
  getCanvas()->clearColorBuffer();
  getCanvas()->flushOutput();
}

void TlistWidget::refresh() {
DOTRACE("TlistWidget::refresh");

  getCanvas()->clearColorBuffer(); 
  safeDrawTrial();
  swapBuffers();
  getCanvas()->flushOutput();
}

void TlistWidget::undraw() {
DOTRACE("TlistWidget::undraw");
  theTlist.getCheckedPtr(itsCurTrial)->trUndraw(*(this->getCanvas()), true);
}

void TlistWidget::setCurTrial(int trial) {
DOTRACE("TlistWidget::setCurTrial");
  if ( !theTlist.isValidId(trial) )
	 { throw InvalidIdError("invalid trial id"); }
	 
  itsCurTrial = trial;
}

void TlistWidget::safeDrawTrial() {
DOTRACE("TlistWidget::safeDrawTrial");

  if ( !itsVisibility ) return;

  if ( theTlist.isValidId(itsCurTrial) ) {
	 try {
		DebugPrintNL("drawing the trial...");
		theTlist.getPtr(itsCurTrial)->trDraw(*(getCanvas()), false);
		return;
	 }
	 catch (InvalidIdError& err) {
		DebugEvalNL(err.msg_cstr());
	 }
  }

  // Here, either the trial id or some other id was invalid
  setVisibility(false);
}
static const char vcid_tlistwidget_cc[] = "$Header$";
#endif // !TLISTWIDGET_CC_DEFINED
