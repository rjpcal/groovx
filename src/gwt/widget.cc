///////////////////////////////////////////////////////////////////////
//
// widget.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 12:52:59 1999
// written: Tue Nov 28 14:09:49 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_CC_DEFINED
#define WIDGET_CC_DEFINED

#include "gwt/widget.h"

#include "trialbase.h"

#include "io/io.h"
#include "io/iditem.h"

#include "gwt/canvas.h"

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// GWT::Widget::Impl definition
//
///////////////////////////////////////////////////////////////////////

class GWT::Widget::Impl {
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
// GWT::Widget::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

void GWT::Widget::Impl::safeDrawTrial(GWT::Canvas& canvas) {
DOTRACE("GWT::Widget::Impl::safeDrawTrial");

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

void GWT::Widget::Impl::display(GWT::Canvas& canvas) {
DOTRACE("GWT::Widget::Impl::display");

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

void GWT::Widget::Impl::clearscreen(GWT::Canvas& canvas) {
DOTRACE("GWT::Widget::Impl::clearscreen");
  setVisibility(false); 
  canvas.clearColorBuffer();
  canvas.flushOutput();
}

void GWT::Widget::Impl::undraw(GWT::Canvas& canvas) {
DOTRACE("GWT::Widget::Impl::undraw");
  itsUndrawItem->trUndraw(canvas, true);
}

///////////////////////////////////////////////////////////////////////
//
// GWT::Widget member definitions
//
///////////////////////////////////////////////////////////////////////

GWT::Widget::Widget() :
  itsImpl(new Impl)
{
DOTRACE("GWT::Widget::Widget");
}

GWT::Widget::~Widget() {
  delete itsImpl;
}

void GWT::Widget::display() {
DOTRACE("GWT::Widget::display");

  itsImpl->display(*(getCanvas()));

  swapBuffers();
  getCanvas()->flushOutput();
}

void GWT::Widget::clearscreen()
  { itsImpl->clearscreen(*(getCanvas())); }

void GWT::Widget::refresh() {
DOTRACE("GWT::Widget::refresh");

  getCanvas()->clearColorBuffer(); 
  itsImpl->safeDrawTrial(*(getCanvas()));
  swapBuffers();
  getCanvas()->flushOutput();
}

void GWT::Widget::undraw()
  { itsImpl->undraw(*(getCanvas())); }

void GWT::Widget::setVisibility(bool vis) {
DOTRACE("GWT::Widget::setVisibility");
  itsImpl->setVisibility(vis);
}

void GWT::Widget::setCurTrial(const MaybeIdItem<TrialBase>& item) {
DOTRACE("GWT::Widget::setCurTrial");
  itsImpl->setCurTrial(item);
}

void GWT::Widget::setHold(bool hold_on) {
DOTRACE("GWT::Widget::setHold");
  itsImpl->setHold(hold_on);
}

static const char vcid_widget_cc[] = "$Header$";
#endif // !WIDGET_CC_DEFINED
