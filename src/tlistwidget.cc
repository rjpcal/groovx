///////////////////////////////////////////////////////////////////////
//
// tlistwidget.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Dec  3 14:46:38 1999
// written: Mon Dec  6 23:37:41 1999
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

  void safeDrawTrial(int trial, TlistWidget* widg) {
  DOTRACE("{tlistwidget.cc}::safeDrawTrial");

	 DebugPrintNL("drawing the trial...");
	 try {
		theTlist.getCheckedPtr(trial)->trDraw(*(widg->getCanvas()), false);
	 }
	 catch (InvalidIdError& err) {
		DebugEvalNL(err.msg());
		widg->setVisibility(false);
	 }
  }
}

TlistWidget::TlistWidget(Togl* togl, double dist, double unit_angle) :
  ToglConfig(togl, dist, unit_angle) 
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

  safeDrawTrial(itsCurTrial, this);
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
  safeDrawTrial(itsCurTrial, this);
  swapBuffers();
  getCanvas()->flushOutput();
}

void TlistWidget::undraw() {
DOTRACE("TlistWidget::undraw");
  theTlist.getCheckedPtr(itsCurTrial)->trUndraw(*(this->getCanvas()), true);
}

void TlistWidget::setCurTrial(int trial) {
DOTRACE("TlistWidget::setCurTrial");
  if (theTlist.isValidId(trial))
	 itsCurTrial = trial;
}

static const char vcid_tlistwidget_cc[] = "$Header$";
#endif // !TLISTWIDGET_CC_DEFINED
