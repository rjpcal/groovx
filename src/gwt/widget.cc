///////////////////////////////////////////////////////////////////////
//
// widget.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 12:52:59 1999
// written: Fri Jul  5 14:11:34 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_CC_DEFINED
#define WIDGET_CC_DEFINED

#include "gwt/widget.h"

#include "gfx/canvas.h"
#include "gfx/gxnode.h"

#include "tcl/tcltimer.h"

#include "util/dlink_list.h"
#include "util/ref.h"
#include "util/volatileobject.h"

#include "util/trace.h"
#include "util/debug.h"

class EmptyNode : public GxNode
{
private:
  EmptyNode() {}
public:
  virtual ~EmptyNode() {}

  virtual void readFrom(IO::Reader*) {}
  virtual void writeTo(IO::Writer*) const {}

  virtual void getBoundingCube(Gfx::Box<double>&, Gfx::Canvas&) const {}

  virtual void draw(Gfx::Canvas&) const {}

  static EmptyNode* make()
    {
      return new EmptyNode;
    }
};

namespace
{
  Util::Ref<EmptyNode> getEmptyNode()
  {
    static Util::Ref<EmptyNode> node(EmptyNode::make());
    return node;
  }
}

///////////////////////////////////////////////////////////////////////
//
// GWT::Widget::Impl definition
//
///////////////////////////////////////////////////////////////////////

class GWT::Widget::Impl : public Util::VolatileObject
{
public:
  Impl(GWT::Widget* owner) :
    itsOwner(owner),
    itsDrawNode(getEmptyNode()),
    itsUndrawNode(getEmptyNode()),
    isItVisible(false),
    isItHolding(false),
    isItRefreshing(true),
    isItRefreshed(false),
    itsTimer(100, true),
    itsButtonListeners(),
    itsKeyListeners(),
    slotNodeChanged(Util::Slot::make(this, &Impl::onNodeChange))
  {
    itsTimer.sigTimeOut.connect(this, &Impl::fullRender);
  }

  static Impl* make(GWT::Widget* owner) { return new Impl(owner); }

  void undraw(Gfx::Canvas& canvas);

  void render();

  void fullRender();

  void clearscreen(Gfx::Canvas& canvas)
  {
    canvas.clearColorBuffer();
    setDrawable(getEmptyNode());
    itsUndrawNode = getEmptyNode();
    isItVisible = false;
  }

  void fullClearscreen(Gfx::Canvas& canvas)
  {
    clearscreen(canvas);
    doFlush(canvas);
  }

  void setVisibility(bool val)
  {
    isItVisible = val;
    if ( !isItVisible )
      {
        fullClearscreen(itsOwner->getCanvas());
      }
  }

  void setDrawable(const Ref<GxNode>& node)
  {
    itsDrawNode->sigNodeChanged.disconnect(slotNodeChanged);

    itsDrawNode = node;

    itsDrawNode->sigNodeChanged.connect(slotNodeChanged);
  }

  void flushChanges()
  {
    if (isItRefreshing && !isItRefreshed)
      fullRender();
  }

  void onNodeChange()
  {
    isItRefreshed = false;
    flushChanges();
  }

private:
  void doFlush(Gfx::Canvas& canvas)
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
  bool isItVisible;

public:
  bool isItHolding;
  bool isItRefreshing;
  bool isItRefreshed;

  Tcl::Timer itsTimer;

  typedef dlink_list<Util::Ref<GWT::ButtonListener> > Buttons;
  typedef dlink_list<Util::Ref<GWT::KeyListener> >    Keys;

  Buttons itsButtonListeners;
  Keys itsKeyListeners;

  Util::Ref<Util::Slot> slotNodeChanged;
};


///////////////////////////////////////////////////////////////////////
//
// GWT::Widget::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

void GWT::Widget::Impl::render()
{
DOTRACE("GWT::Widget::Impl::render");

  Gfx::Canvas& canvas = itsOwner->getCanvas();

  try
    {
      Gfx::MatrixSaver msaver(canvas);
      Gfx::AttribSaver asaver(canvas);

      itsDrawNode->draw(canvas);
      itsUndrawNode = itsDrawNode;

      isItRefreshed = true;
    }
  catch (...)
    {
      // Here, something failed during rendering, so just go invisible
      setVisibility(false);
    }
}

void GWT::Widget::Impl::fullRender()
{
DOTRACE("GWT::Widget::Impl::fullRender");

  Gfx::Canvas& canvas = itsOwner->getCanvas();

  // (1) Clear the screen (but only if we are not "holding")
  if( !isItHolding )
    {
      canvas.clearColorBuffer();
    }

  // (2) Render the current object
  if ( isItVisible )
    {
      render();
    }

  // (3) Flush the graphics stream
  doFlush(canvas);
}

void GWT::Widget::Impl::undraw(Gfx::Canvas& canvas)
{
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
}

GWT::Widget::~Widget()
{
DOTRACE("GWT::Widget::~Widget");

  itsImpl->destroy();
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
  return !(itsImpl->itsButtonListeners.is_empty());
}

bool GWT::Widget::hasKeyListeners() const
{
  return !(itsImpl->itsKeyListeners.is_empty());
}


void GWT::Widget::removeButtonListeners()
{
  itsImpl->itsButtonListeners.clear();
}

void GWT::Widget::removeKeyListeners()
{
  itsImpl->itsKeyListeners.clear();
}

void GWT::Widget::render()
{
  itsImpl->render();
}

void GWT::Widget::fullRender()
{
  itsImpl->fullRender();
}

void GWT::Widget::clearscreen()
{
  itsImpl->clearscreen(getCanvas());
}

void GWT::Widget::fullClearscreen()
{
  itsImpl->fullClearscreen(getCanvas());
}

void GWT::Widget::undraw()
{
  itsImpl->undraw(getCanvas());
}

void GWT::Widget::setVisibility(bool vis)
{
DOTRACE("GWT::Widget::setVisibility");
  itsImpl->setVisibility(vis);
}

void GWT::Widget::setHold(bool hold_on)
{
DOTRACE("GWT::Widget::setHold");
  itsImpl->isItHolding = hold_on;
}

void GWT::Widget::allowRefresh(bool allow)
{
DOTRACE("GWT::Widget::allowRefresh");
  itsImpl->isItRefreshing = allow;
  itsImpl->flushChanges();
}

void GWT::Widget::setDrawable(const Ref<GxNode>& node)
{
DOTRACE("GWT::Widget::setDrawable");
  itsImpl->setDrawable(node);
}

void GWT::Widget::animate(unsigned int framesPerSecond)
{
DOTRACE("GWT::Widget::animate");
  if (framesPerSecond == 0)
    {
      itsImpl->itsTimer.cancel();
    }
  else
    {
      itsImpl->itsTimer.setDelayMsec(1000/framesPerSecond);
      itsImpl->itsTimer.schedule();
    }
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
