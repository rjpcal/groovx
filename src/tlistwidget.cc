///////////////////////////////////////////////////////////////////////
//
// tlistwidget.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Dec  3 14:46:38 1999
// written: Fri Nov 10 17:03:58 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTWIDGET_CC_DEFINED
#define TLISTWIDGET_CC_DEFINED

#include "tlistwidget.h"

#include "trialbase.h"

#include "io/io.h"
#include "io/iditem.h"

#include "gwt/canvas.h"

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TlistWidget::Impl definition
//
///////////////////////////////////////////////////////////////////////

class TlistWidget::Impl {
public:
  Impl() :
	 itsDrawItem(0),
	 itsUndrawItem(0),
	 itsVisibility(false),
	 itsHoldOn(false)
  {}

  void safeDrawTrial(GWT::Canvas& canvas);

  void display(GWT::Canvas& canvas);

  void clearscreen(GWT::Canvas& canvas);

  void undraw(GWT::Canvas& canvas);

  void setVisibility(bool val)
  {
	 itsVisibility = val;
	 if (itsVisibility == false)
		{
		  itsDrawItem = MaybeIdItem<TrialBase>(itsDrawItem.id());
		  itsUndrawItem =  MaybeIdItem<TrialBase>(itsUndrawItem.id());
		}
  }

  void setCurTrial(const MaybeIdItem<TrialBase>& item)
  {
  	 itsDrawItem = item;

	 try {
		itsDrawItem.refresh();
	 }
	 catch(...) {
		setVisibility(false);
		throw;
	 }
  }

  void setHold(bool hold_on)
  { itsHoldOn = hold_on; }

private:
  MaybeIdItem<TrialBase> itsDrawItem;
  MaybeIdItem<TrialBase> itsUndrawItem;
  bool itsVisibility;
  bool itsHoldOn;
};


///////////////////////////////////////////////////////////////////////
//
// TlistWidget::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

void TlistWidget::Impl::safeDrawTrial(GWT::Canvas& canvas) {
DOTRACE("TlistWidget::Impl::safeDrawTrial");

  if ( !itsVisibility ) return;

  try {
	 DebugPrintNL("drawing the trial...");
	 itsDrawItem->trDraw(canvas, false);
	 itsUndrawItem = itsDrawItem;
	 return;
  }
  catch (ErrorWithMsg& err) {
	 DebugEvalNL(err.msg_cstr());
  }

  // Here, either the trial id or some other id was invalid
  setVisibility(false);
}

void TlistWidget::Impl::display(GWT::Canvas& canvas) {
DOTRACE("TlistWidget::Impl::display");

  // Only erase the previous trial if hold is off
  if( !itsHoldOn ) {
	 // First we must erase the previous current trial. We ignore any
	 // invalid id errors that occur, and simply clear the screen in
	 // this case.
	 try {
		itsUndrawItem->trUndraw(canvas, false);
	 }
	 catch (...) {
		canvas.clearColorBuffer();
	 }
  }

  safeDrawTrial(canvas);
}

void TlistWidget::Impl::clearscreen(GWT::Canvas& canvas) {
DOTRACE("TlistWidget::Impl::clearscreen");
  setVisibility(false); 
  canvas.clearColorBuffer();
  canvas.flushOutput();
}

void TlistWidget::Impl::undraw(GWT::Canvas& canvas) {
DOTRACE("TlistWidget::Impl::undraw");
  itsUndrawItem->trUndraw(canvas, true);
}

///////////////////////////////////////////////////////////////////////
//
// TlistWidget member definitions
//
///////////////////////////////////////////////////////////////////////

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

  itsImpl->display(*(getCanvas()));

  swapBuffers();
  getCanvas()->flushOutput();
}

void TlistWidget::clearscreen()
  { itsImpl->clearscreen(*(getCanvas())); }

void TlistWidget::refresh() {
DOTRACE("TlistWidget::refresh");

  getCanvas()->clearColorBuffer(); 
  itsImpl->safeDrawTrial(*(getCanvas()));
  swapBuffers();
  getCanvas()->flushOutput();
}

void TlistWidget::undraw()
  { itsImpl->undraw(*(getCanvas())); }

void TlistWidget::setVisibility(bool vis) {
DOTRACE("TlistWidget::setVisibility");
  itsImpl->setVisibility(vis);
}

void TlistWidget::setCurTrial(const MaybeIdItem<TrialBase>& item) {
DOTRACE("TlistWidget::setCurTrial");
  itsImpl->setCurTrial(item);
}

void TlistWidget::setHold(bool hold_on) {
DOTRACE("TlistWidget::setHold");
  itsImpl->setHold(hold_on);
}

static const char vcid_tlistwidget_cc[] = "$Header$";
#endif // !TLISTWIDGET_CC_DEFINED
