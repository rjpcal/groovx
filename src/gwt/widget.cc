///////////////////////////////////////////////////////////////////////
//
// widget.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 12:52:59 1999
// written: Sat Nov 23 14:21:19 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_CC_DEFINED
#define WIDGET_CC_DEFINED

#include "gwt/widget.h"

#include "gfx/canvas.h"
#include "gfx/gxcamera.h"
#include "gfx/gxemptynode.h"
#include "gfx/gxnode.h"

#include "tcl/tcltimer.h"

#include "util/dlink_list.h"
#include "util/ref.h"
#include "util/volatileobject.h"

#include "util/trace.h"
#include "util/debug.h"

class Scene : public Util::VolatileObject
{
public:
  Scene(GWT::Widget* owner) :
    itsDrawNode(GxEmptyNode::make()),
    itsUndrawNode(GxEmptyNode::make()),
    isItVisible(false),
    itsCamera(new GxFixedScaleCamera()),
    isItHolding(false),
    isItRefreshing(true),
    isItRefreshed(false),
    itsTimer(100, true),
    slotNodeChanged(Util::Slot::make(owner, &GWT::Widget::onNodeChange))
  {
    itsTimer.sigTimeOut.connect(owner, &GWT::Widget::fullRender);
    itsCamera->sigNodeChanged.connect(slotNodeChanged);
  }

  static Scene* make(GWT::Widget* owner) { return new Scene(owner); }

  void undraw(Gfx::Canvas& canvas);

  void render(Gfx::Canvas& canvas, int width, int height);

  void fullRender(Gfx::Canvas& canvas, int width, int height);

  void clearscreen(Gfx::Canvas& canvas)
  {
    canvas.clearColorBuffer();
    setDrawable(Util::Ref<GxNode>(GxEmptyNode::make()));
    itsUndrawNode = Util::Ref<GxNode>(GxEmptyNode::make());
    isItVisible = false;
  }

  void fullClearscreen(Gfx::Canvas& canvas)
  {
    clearscreen(canvas);
    doFlush(canvas);
  }

  void setVisibility(Gfx::Canvas& canvas, bool val)
  {
    isItVisible = val;
    if ( !isItVisible )
      {
        fullClearscreen(canvas);
      }
  }

  void setCamera(const Ref<GxCamera>& cam)
  {
    itsCamera->sigNodeChanged.disconnect(slotNodeChanged);

    itsCamera = cam;

    itsCamera->sigNodeChanged.connect(slotNodeChanged);
  }

  void setDrawable(const Ref<GxNode>& node)
  {
    itsDrawNode->sigNodeChanged.disconnect(slotNodeChanged);

    itsDrawNode = node;

    itsDrawNode->sigNodeChanged.connect(slotNodeChanged);
  }

  void flushChanges(Gfx::Canvas& canvas, int width, int height)
  {
    if (isItRefreshing && !isItRefreshed)
      fullRender(canvas, width, height);
  }

  void onNodeChange(Gfx::Canvas& canvas, int width, int height)
  {
    isItRefreshed = false;
    flushChanges(canvas, width, height);
  }

private:
  void doFlush(Gfx::Canvas& canvas)
  {
    canvas.flushOutput();
  }

  Scene(const Scene&);
  Scene& operator=(const Scene&);

  Util::Ref<GxNode> itsDrawNode;
  Util::Ref<GxNode> itsUndrawNode;
  bool isItVisible;

public:
  Util::Ref<GxCamera> itsCamera;
  bool isItHolding;
  bool isItRefreshing;
  bool isItRefreshed;

  Tcl::Timer itsTimer;

  Util::Ref<Util::Slot> slotNodeChanged;
};

///////////////////////////////////////////////////////////////////////
//
// Scene member definitions
//
///////////////////////////////////////////////////////////////////////

void Scene::render(Gfx::Canvas& canvas, int width, int height)
{
DOTRACE("Scene::render");

  try
    {
      Gfx::MatrixSaver msaver(canvas);
      Gfx::AttribSaver asaver(canvas);

      itsCamera->reshape(width, height);
      itsCamera->draw(canvas);
      itsDrawNode->draw(canvas);
      itsUndrawNode = itsDrawNode;

      isItRefreshed = true;
    }
  catch (...)
    {
      // Here, something failed during rendering, so just go invisible
      setVisibility(canvas, false);
    }
}

void Scene::fullRender(Gfx::Canvas& canvas, int width, int height)
{
DOTRACE("Scene::fullRender");

  // (1) Clear the screen (but only if we are not "holding")
  if( !isItHolding )
    {
      canvas.clearColorBuffer();
    }

  // (2) Render the current object
  if ( isItVisible )
    {
      render(canvas, width, height);
    }

  // (3) Flush the graphics stream
  doFlush(canvas);
}

void Scene::undraw(Gfx::Canvas& canvas)
{
DOTRACE("Scene::undraw");
  itsUndrawNode->undraw(canvas);
  doFlush(canvas);
}

///////////////////////////////////////////////////////////////////////
//
// GWT::Widget::Impl definition
//
///////////////////////////////////////////////////////////////////////

class GWT::Widget::Impl
{
public:
  Impl() :
    itsButtonListeners(),
    itsKeyListeners()
  {}

  typedef dlink_list<Util::Ref<GWT::ButtonListener> > Buttons;
  typedef dlink_list<Util::Ref<GWT::KeyListener> >    Keys;

  Buttons itsButtonListeners;
  Keys itsKeyListeners;
};

///////////////////////////////////////////////////////////////////////
//
// GWT::Widget member definitions
//
///////////////////////////////////////////////////////////////////////

GWT::Widget::Widget() :
  itsImpl(new Impl),
  itsScene(Scene::make(this))
{
DOTRACE("GWT::Widget::Widget");
}

GWT::Widget::~Widget()
{
DOTRACE("GWT::Widget::~Widget");

  itsScene->destroy();
  delete itsImpl;
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
  itsScene->render(getCanvas(), width(), height());
}

void GWT::Widget::fullRender()
{
  itsScene->fullRender(getCanvas(), width(), height());
}

void GWT::Widget::clearscreen()
{
  itsScene->clearscreen(getCanvas());
}

void GWT::Widget::fullClearscreen()
{
  itsScene->fullClearscreen(getCanvas());
}

void GWT::Widget::undraw()
{
  itsScene->undraw(getCanvas());
}

void GWT::Widget::setVisibility(bool vis)
{
DOTRACE("GWT::Widget::setVisibility");
  itsScene->setVisibility(getCanvas(), vis);
}

void GWT::Widget::setHold(bool hold_on)
{
DOTRACE("GWT::Widget::setHold");
  itsScene->isItHolding = hold_on;
}

void GWT::Widget::allowRefresh(bool allow)
{
DOTRACE("GWT::Widget::allowRefresh");
  itsScene->isItRefreshing = allow;
  itsScene->flushChanges(getCanvas(), width(), height());
}

const Util::Ref<GxCamera>& GWT::Widget::getCamera() const
{
DOTRACE("GWT::Widget::getCamera");

  return itsScene->itsCamera;
}

void GWT::Widget::setCamera(const Util::Ref<GxCamera>& cam)
{
DOTRACE("GWT::Widget::setCamera");

  itsScene->setCamera(cam);
  fullRender();
}

void GWT::Widget::setDrawable(const Ref<GxNode>& node)
{
DOTRACE("GWT::Widget::setDrawable");
  itsScene->setDrawable(node);
}

void GWT::Widget::animate(unsigned int framesPerSecond)
{
DOTRACE("GWT::Widget::animate");
  if (framesPerSecond == 0)
    {
      itsScene->itsTimer.cancel();
    }
  else
    {
      itsScene->itsTimer.setDelayMsec(1000/framesPerSecond);
      itsScene->itsTimer.schedule();
    }
}

void GWT::Widget::onNodeChange()
{
  itsScene->onNodeChange(getCanvas(), width(), height());
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
