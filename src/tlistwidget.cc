///////////////////////////////////////////////////////////////////////
//
// tlistwidget.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Dec  3 14:46:38 1999
// written: Tue Oct 17 16:24:18 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTWIDGET_CC_DEFINED
#define TLISTWIDGET_CC_DEFINED

#include "tlistwidget.h"

#include "tlist.h"
#include "trialbase.h"

#include "gwt/canvas.h"

#include "util/trace.h"
#include "util/debug.h"


namespace {
  Tlist& theTlist = Tlist::theTlist();
}

///////////////////////////////////////////////////////////////////////
//
// TlistWidget::Impl definition
//
///////////////////////////////////////////////////////////////////////

class TlistWidget::Impl {
public:
  Impl() :
	 itsCurTrial(0),
	 itsVisibility(false),
	 itsHoldOn(false)
  {}

  int itsCurTrial;
  bool itsVisibility;
  bool itsHoldOn;
};

TlistWidget::TlistWidget(Tcl_Interp* interp, const char* pathname,
								 int config_argc, char** config_argv,
								 double dist, double unit_angle) :
  ToglConfig(interp, pathname, config_argc, config_argv, dist, unit_angle),
  itsImpl(new Impl)
{
DOTRACE("TlistWidget::TlistWidget");
}

TlistWidget::~TlistWidget() {
  delete itsImpl;
}

void TlistWidget::display() {
DOTRACE("TlistWidget::display");

  // Only erase the previous trial if hold is off
  if( !itsImpl->itsHoldOn ) {
	 // First we must erase the previous current trial. We ignore any
	 // invalid id errors that occur, and simply clear the screen in
	 // this case.
	 try {
		theTlist.getCheckedPtr(itsImpl->itsCurTrial)->trUndraw(*(this->getCanvas()), false);
	 }
	 catch (InvalidIdError&) {
		clearscreen();
	 }
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
  theTlist.getCheckedPtr(itsImpl->itsCurTrial)->trUndraw(*(this->getCanvas()), true);
}

void TlistWidget::setVisibility(bool vis) {
DOTRACE("TlistWidget::setVisibility");
  itsImpl->itsVisibility = vis;
}

void TlistWidget::setCurTrial(const NullableItemWithId<TrialBase>& item) {
DOTRACE("TlistWidget::setCurTrial");
  if ( !theTlist.isValidId(item.id()) )
	 { throw InvalidIdError("invalid trial id"); }
	 
  itsImpl->itsCurTrial = item.id();
}

void TlistWidget::setHold(bool hold_on) {
DOTRACE("TlistWidget::setHold");
  itsImpl->itsHoldOn = hold_on;
}

void TlistWidget::safeDrawTrial() {
DOTRACE("TlistWidget::safeDrawTrial");

  if ( !itsImpl->itsVisibility ) return;

  if ( theTlist.isValidId(itsImpl->itsCurTrial) ) {
	 try {
		DebugPrintNL("drawing the trial...");
		theTlist.getPtr(itsImpl->itsCurTrial)->trDraw(*(getCanvas()), false);
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
