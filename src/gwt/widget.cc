///////////////////////////////////////////////////////////////////////
//
// widget.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 12:52:59 1999
// written: Wed Aug  8 12:27:25 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_CC_DEFINED
#define WIDGET_CC_DEFINED

#include "gwt/widget.h"

#include "gwt/canvas.h"

#include "gx/gxnode.h"

#include "util/dlink_list.h"
#include "util/observer.h"
#include "util/ref.h"

#define NO_TRACE
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

class GWT::Widget::Impl : public Util::Observer {
public:
  Impl(GWT::Widget* owner) :
    itsOwner(owner),
    itsDrawNode(EmptyNode::make()),
    itsUndrawNode(EmptyNode::make()),
    itsVisibility(false),
    itsHoldOn(false),
    itsButtonListeners(),
    itsKeyListeners()
  {}

  virtual bool isVolatile() const { return true; }

  static Impl* make(GWT::Widget* owner) { return new Impl(owner); }

  void safeDraw(GWT::Canvas& canvas);

  void display(GWT::Canvas& canvas);

  void clearscreen(GWT::Canvas& canvas);

  void refresh(GWT::Canvas& canvas)
    {
      DOTRACE("GWT::Widget::Impl::refresh");
      canvas.clearColorBuffer();
      safeDraw(canvas);
      doFlush(canvas);
    }

  void undraw(GWT::Canvas& canvas);

  void setVisibility(bool val)
    {
      itsVisibility = val;
      if (itsVisibility == false)
        {
          undraw(itsOwner->getCanvas());
          setDrawable(Util::Ref<GxNode>(EmptyNode::make()));
          itsUndrawNode = Util::Ref<GxNode>(EmptyNode::make());
        }
    }

  void setHold(bool hold_on)
    { itsHoldOn = hold_on; }

  void setDrawable(const Ref<GxNode>& node)
    {
      itsDrawNode->detach(this);

      itsDrawNode = node;

      itsDrawNode->attach(this);
    }

  virtual void receiveStateChangeMsg(const Util::Observable*)
    {
      refresh(itsOwner->getCanvas());
    }

  virtual void receiveDestroyMsg(const Util::Observable*) {}

private:
  void doFlush(GWT::Canvas& canvas)
    {
      if (canvas.isDoubleBuffered())
        {
          itsOwner->swapBuffers();
        }
      else
        {
          canvas.flushOutput();
        }
    }

  Impl(const Impl&);
  Impl& operator=(const Impl&);

  GWT::Widget* itsOwner;
  Util::Ref<GxNode> itsDrawNode;
  Util::Ref<GxNode> itsUndrawNode;
  bool itsVisibility;
  bool itsHoldOn;

public:

  typedef dlink_list<Util::Ref<GWT::ButtonListener> > Buttons;
  typedef dlink_list<Util::Ref<GWT::KeyListener> >    Keys;

  Buttons itsButtonListeners;
  Keys itsKeyListeners;
};


///////////////////////////////////////////////////////////////////////
//
// GWT::Widget::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

void GWT::Widget::Impl::safeDraw(GWT::Canvas& canvas) {
DOTRACE("GWT::Widget::Impl::safeDraw");

  if ( !itsVisibility ) return;

  try
    {
      DebugPrintNL("drawing the node...");
      itsDrawNode->draw(canvas);
      itsUndrawNode = itsDrawNode;
      return;
    }
  catch (Util::Error& err)
    {
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
  doFlush(canvas);
}

void GWT::Widget::Impl::clearscreen(GWT::Canvas& canvas) {
DOTRACE("GWT::Widget::Impl::clearscreen");
  setVisibility(false);
  canvas.clearColorBuffer();
  doFlush(canvas);
}

void GWT::Widget::Impl::undraw(GWT::Canvas& canvas) {
DOTRACE("GWT::Widget::Impl::undraw");
  itsUndrawNode->undraw(canvas);
  doFlush(canvas);
}

///////////////////////////////////////////////////////////////////////
//
// GWT::Widget member definitions
//
///////////////////////////////////////////////////////////////////////

GWT::Widget::Widget() :
  itsImpl(Impl::make(this))
{
DOTRACE("GWT::Widget::Widget");

  itsImpl->incrRefCount();
}

GWT::Widget::~Widget()
{
DOTRACE("GWT::Widget::~Widget");

  itsImpl->decrRefCount();
}

void GWT::Widget::addButtonListener(Util::Ref<GWT::ButtonListener> b)
{
  itsImpl->itsButtonListeners.push_back(b);
}

void GWT::Widget::addKeyListener(Util::Ref<GWT::KeyListener> k)
{
  itsImpl->itsKeyListeners.push_back(k);
}

bool GWT::Widget::hasButtonListeners() const
{
  return !(itsImpl->itsButtonListeners.empty());
}

bool GWT::Widget::hasKeyListeners() const
{
  return !(itsImpl->itsKeyListeners.empty());
}


void GWT::Widget::removeButtonListeners()
{
  itsImpl->itsButtonListeners.clear();
}

void GWT::Widget::removeKeyListeners()
{
  itsImpl->itsKeyListeners.clear();
}

void GWT::Widget::display()
{
  itsImpl->display(getCanvas());
}

void GWT::Widget::clearscreen()
{
  itsImpl->clearscreen(getCanvas());
}

void GWT::Widget::refresh()
{
  itsImpl->refresh(getCanvas());
}

void GWT::Widget::undraw()
{
  itsImpl->undraw(getCanvas());
}

void GWT::Widget::setVisibility(bool vis) {
DOTRACE("GWT::Widget::setVisibility");
  itsImpl->setVisibility(vis);
}

void GWT::Widget::setHold(bool hold_on) {
DOTRACE("GWT::Widget::setHold");
  itsImpl->setHold(hold_on);
}

void GWT::Widget::setDrawable(const Ref<GxNode>& node) {
DOTRACE("GWT::Widget::setDrawable");
  itsImpl->setDrawable(node);
}

void GWT::Widget::dispatchButtonEvent(unsigned int button, int x, int y)
{
  for (Impl::Buttons::iterator
         itr = itsImpl->itsButtonListeners.begin(),
         end = itsImpl->itsButtonListeners.end();
       itr != end;
       ++itr)
    {
      EventStatus status = (*itr)->onButtonPress(button, x, y);
      if (status == HANDLED)
        break;
    }
}

void GWT::Widget::dispatchKeyEvent(const char* keys, int x, int y,
                                   bool controlPressed)
{
  for (Impl::Keys::iterator
         itr = itsImpl->itsKeyListeners.begin(),
         end = itsImpl->itsKeyListeners.end();
       itr != end;
       ++itr)
    {
      EventStatus status = (*itr)->onKeyPress(keys, x, y, controlPressed);
      if (status == HANDLED)
        break;
    }
}

static const char vcid_widget_cc[] = "$Header$";
#endif // !WIDGET_CC_DEFINED
