///////////////////////////////////////////////////////////////////////
//
// tlistwidget.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Dec  3 14:46:38 1999
// written: Mon Dec  6 20:18:55 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTWIDGET_CC_DEFINED
#define TLISTWIDGET_CC_DEFINED

#include "tlistwidget.h"

#include <GL/gl.h>

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
		theTlist.getCheckedPtr(trial)->trDraw(Canvas::theCanvas(), false);
	 }
	 catch (InvalidIdError& err) {
		DebugEvalNL(err.msg());
		widg->setVisibility(false);
	 }
  }

  void clearColorBuffer() {
  DOTRACE("{tlistwidget.cc}::clearColorBuffer");
    glClear(GL_COLOR_BUFFER_BIT);
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
	 theTlist.getCheckedPtr(itsCurTrial)->trUndraw(Canvas::theCanvas(), false);
  }
  catch (InvalidIdError&) {
	 clearscreen();
  }

  safeDrawTrial(itsCurTrial, this);
  glFlush();
}

void TlistWidget::clearscreen() {
DOTRACE("TlistWidget::clearscreen");
  setVisibility(false);
  clearColorBuffer();
  glFlush();
}

void TlistWidget::refresh() {
DOTRACE("TlistWidget::refresh");

  clearColorBuffer(); 
  safeDrawTrial(itsCurTrial, this);
  swapBuffers();
  glFlush();
}

void TlistWidget::undraw() {
DOTRACE("TlistWidget::undraw");
  theTlist.getCheckedPtr(itsCurTrial)->trUndraw(Canvas::theCanvas(), true);
}

void TlistWidget::setCurTrial(int trial) {
DOTRACE("TlistWidget::setCurTrial");
  if (theTlist.isValidId(trial))
	 itsCurTrial = trial;
}

static const char vcid_tlistwidget_cc[] = "$Header$";
#endif // !TLISTWIDGET_CC_DEFINED
