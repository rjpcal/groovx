///////////////////////////////////////////////////////////////////////
//
// widget.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 12:52:59 1999
// written: Thu May 10 12:04:44 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_CC_DEFINED
#define WIDGET_CC_DEFINED

#include "gwt/widget.h"

#include "gx/gxnode.h"

#include "io/io.h"
#include "io/iditem.h"

#include "gwt/canvas.h"

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

class EmptyNode : public GxNode {
private:
  EmptyNode() {}
public:
  virtual ~EmptyNode() {}

  virtual void readFrom(IO::Reader*) {}
  virtual void writeTo(IO::Writer*) const {}

  virtual void draw(GWT::Canvas&) const {}
  virtual void undraw(GWT::Canvas&) const {}

  static EmptyNode* make()
  {
	 static EmptyNode* p = 0;
	 if (p == 0)
		{
		  p = new EmptyNode;
		  p->incrRefCount();
		}
	 return p;
  }
};

///////////////////////////////////////////////////////////////////////
//
// GWT::Widget::Impl definition
//
///////////////////////////////////////////////////////////////////////

class GWT::Widget::Impl {
public:
  Impl(GWT::Widget* owner) :
	 itsOwner(owner),
    itsDrawNode(EmptyNode::make()),
    itsUndrawNode(EmptyNode::make()),
	 itsVisibility(false),
	 itsHoldOn(false)
  {}

  void safeDraw(GWT::Canvas& canvas);

  void display(GWT::Canvas& canvas);

  void clearscreen(GWT::Canvas& canvas);

  void undraw(GWT::Canvas& canvas);

  void setVisibility(bool val)
  {
	 itsVisibility = val;
  	 if (itsVisibility == false)
  		{
		  undraw(*(itsOwner->getCanvas()));
		  itsDrawNode = PtrHandle<GxNode>(EmptyNode::make());
		  itsUndrawNode = PtrHandle<GxNode>(EmptyNode::make());
  		}
  }

  void setHold(bool hold_on)
  { itsHoldOn = hold_on; }

  void setDrawable(const IdItem<GxNode>& node)
  { itsDrawNode = node.handle(); }

private:
  GWT::Widget* itsOwner;
  PtrHandle<GxNode> itsDrawNode;
  PtrHandle<GxNode> itsUndrawNode;
  bool itsVisibility;
  bool itsHoldOn;
};


///////////////////////////////////////////////////////////////////////
//
// GWT::Widget::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

void GWT::Widget::Impl::safeDraw(GWT::Canvas& canvas) {
DOTRACE("GWT::Widget::Impl::safeDraw");

  if ( !itsVisibility ) return;

  try {
	 DebugPrintNL("drawing the node...");
	 itsDrawNode->draw(canvas);
	 itsUndrawNode = itsDrawNode;
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
		itsUndrawNode->undraw(canvas);
	 }
	 catch (...) {
		canvas.clearColorBuffer();
	 }
  }

  safeDraw(canvas);
}

void GWT::Widget::Impl::clearscreen(GWT::Canvas& canvas) {
DOTRACE("GWT::Widget::Impl::clearscreen");
  setVisibility(false); 
  canvas.clearColorBuffer();
  canvas.flushOutput();
}

void GWT::Widget::Impl::undraw(GWT::Canvas& canvas) {
DOTRACE("GWT::Widget::Impl::undraw");
  itsUndrawNode->undraw(canvas); 
  canvas.flushOutput();
}

///////////////////////////////////////////////////////////////////////
//
// GWT::Widget member definitions
//
///////////////////////////////////////////////////////////////////////

GWT::Widget::Widget() :
  itsImpl(new Impl(this))
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
  itsImpl->safeDraw(*(getCanvas()));
  swapBuffers();
  getCanvas()->flushOutput();
}

void GWT::Widget::undraw()
  { itsImpl->undraw(*(getCanvas())); }

void GWT::Widget::setVisibility(bool vis) {
DOTRACE("GWT::Widget::setVisibility");
  itsImpl->setVisibility(vis);
}

void GWT::Widget::setHold(bool hold_on) {
DOTRACE("GWT::Widget::setHold");
  itsImpl->setHold(hold_on);
}

void GWT::Widget::setDrawable(const IdItem<GxNode>& node) {
DOTRACE("GWT::Widget::setDrawable");
  itsImpl->setDrawable(node);
}

static const char vcid_widget_cc[] = "$Header$";
#endif // !WIDGET_CC_DEFINED
