///////////////////////////////////////////////////////////////////////
//
// tlistwidget.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Dec  3 14:46:38 1999
// written: Fri Dec  3 14:52:49 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTWIDGET_CC_DEFINED
#define TLISTWIDGET_CC_DEFINED

#include "tlistwidget.h"

#include <GL/gl.h>

#include "tlist.h"

#include "trace.h"
#include "debug.h"

TlistWidget::TlistWidget(Togl* togl, double dist, double unit_angle) :
  ToglConfig(togl, dist, unit_angle) 
{
DOTRACE("TlistWidget::TlistWidget");
}

void TlistWidget::display() {
DOTRACE("TlistWidget::display");
  DebugPrintNL("clearing back buffer...");
  glClear(GL_COLOR_BUFFER_BIT);
    
  DebugPrintNL("drawing the trial...");
  try {
	 Tlist::theTlist().drawCurTrial();
  }
  catch (InvalidIdError& err) {
	 DebugEvalNL(err.msg());
	 Tlist::theTlist().setVisible(false);
  }
  
  DebugPrintNL("swapping the buffers...");
  swapBuffers();
  
  DebugPrintNL("clearing back buffer...");
  glClear(GL_COLOR_BUFFER_BIT);
}

static const char vcid_tlistwidget_cc[] = "$Header$";
#endif // !TLISTWIDGET_CC_DEFINED
