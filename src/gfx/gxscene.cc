///////////////////////////////////////////////////////////////////////
//
// gxscene.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Nov 23 17:42:51 2002
// written: Tue Nov 26 19:10:01 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXSCENE_CC_DEFINED
#define GXSCENE_CC_DEFINED

#include "gxscene.h"

#include "util/debug.h"
#include "util/trace.h"

///////////////////////////////////////////////////////////////////////
//
// GxScene member definitions
//
///////////////////////////////////////////////////////////////////////

GxScene::GxScene(Util::SoftRef<Gfx::Canvas> canvas) :
  itsCanvas(canvas),
  itsDrawNode(GxEmptyNode::make()),
  itsUndrawNode(GxEmptyNode::make()),
  isItVisible(false),
  itsCamera(new GxFixedScaleCamera()),
  itsWidth(0),
  itsHeight(0),
  isItHolding(false),
  isItRefreshing(true),
  isItRefreshed(false),
  itsTimer(100, true),
  slotNodeChanged(Util::Slot0::make(this, &GxScene::onNodeChange))
{
DOTRACE("GxScene::GxScene");
  itsTimer.sigTimeOut.connect(this, &GxScene::fullRender);
  itsCamera->sigNodeChanged.connect(slotNodeChanged);
}

void GxScene::render()
{
DOTRACE("GxScene::render");

  try
    {
      Gfx::MatrixSaver msaver(*itsCanvas);
      Gfx::AttribSaver asaver(*itsCanvas);

      itsCamera->draw(*itsCanvas);
      itsDrawNode->draw(*itsCanvas);
      itsUndrawNode = itsDrawNode;

      isItRefreshed = true;
    }
  catch (...)
    {
      // Here, something failed during rendering, so just go invisible
      setVisibility(false);
    }
}

void GxScene::fullRender()
{
DOTRACE("GxScene::fullRender");

  // (1) Clear the screen (but only if we are not "holding")
  if( !isItHolding )
    {
      itsCanvas->clearColorBuffer();
    }

  // (2) Render the current object
  if ( isItVisible )
    {
      render();
    }

  // (3) Flush the graphics stream
  itsCanvas->flushOutput();
}

void GxScene::undraw()
{
DOTRACE("GxScene::undraw");
  itsUndrawNode->undraw(*itsCanvas);
  itsCanvas->flushOutput();
}

void GxScene::clearscreen()
{
DOTRACE("GxScene::clearscreen");
  itsCanvas->clearColorBuffer();
  setDrawable(Util::Ref<GxNode>(GxEmptyNode::make()));
  itsUndrawNode = Util::Ref<GxNode>(GxEmptyNode::make());
  isItVisible = false;
}

void GxScene::fullClearscreen()
{
DOTRACE("GxScene::fullClearscreen");
  clearscreen();
  itsCanvas->flushOutput();
}

void GxScene::setVisibility(bool val)
{
DOTRACE("GxScene::setVisibility");
  isItVisible = val;
  if ( !isItVisible )
    {
      fullClearscreen();
    }
}

void GxScene::setCamera(const Ref<GxCamera>& cam)
{
DOTRACE("GxScene::setCamera");
  itsCamera->sigNodeChanged.disconnect(slotNodeChanged);

  itsCamera = cam;

  itsCamera->reshape(itsWidth, itsHeight);

  itsCamera->sigNodeChanged.connect(slotNodeChanged);

  fullRender();
}

void GxScene::setDrawable(const Ref<GxNode>& node)
{
DOTRACE("GxScene::setDrawable");
  itsDrawNode->sigNodeChanged.disconnect(slotNodeChanged);

  itsDrawNode = node;

  itsDrawNode->sigNodeChanged.connect(slotNodeChanged);
}

void GxScene::flushChanges()
{
DOTRACE("GxScene::flushChanges");
  if (isItRefreshing && !isItRefreshed)
    fullRender();
}

void GxScene::onNodeChange()
{
DOTRACE("GxScene::onNodeChange");
  isItRefreshed = false;
  flushChanges();
}

void GxScene::reshape(int width, int height)
{
DOTRACE("GxScene::reshape");
  itsWidth = width;
  itsHeight = height;
  itsCamera->reshape(itsWidth, itsHeight);
}

void GxScene::animate(unsigned int framesPerSecond)
{
DOTRACE("GxScene::animate");
  if (framesPerSecond == 0)
    {
      itsTimer.cancel();
    }
  else
    {
      itsTimer.setDelayMsec((unsigned int)(1000.0/framesPerSecond));
      itsTimer.schedule();
    }
}

static const char vcid_gxscene_cc[] = "$Header$";
#endif // !GXSCENE_CC_DEFINED
