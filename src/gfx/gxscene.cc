///////////////////////////////////////////////////////////////////////
//
// gxscene.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Nov 23 17:42:51 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_GFX_GXSCENE_CC_UTC20050626084025_DEFINED
#define GROOVX_GFX_GXSCENE_CC_UTC20050626084025_DEFINED

#include "gxscene.h"

#include "nub/scheduler.h"

#include "tcl/tcltimerscheduler.h"

#include "rutz/sharedptr.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// GxScene member definitions
//
///////////////////////////////////////////////////////////////////////

GxScene::GxScene(Nub::SoftRef<Gfx::Canvas> canvas) :
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
  itsScheduler(rutz::make_shared(new Tcl::TimerScheduler)),
  itsTimer(100, true),
  slotNodeChanged(Nub::Slot0::make(this, &GxScene::onNodeChange))
{
GVX_TRACE("GxScene::GxScene");
  itsTimer.sigTimeOut.connect(this, &GxScene::fullRender);
  itsCamera->sigNodeChanged.connect(slotNodeChanged);
}

GxScene::~GxScene() throw()
{
GVX_TRACE("GxScene::~GxScene");
}

void GxScene::render()
{
GVX_TRACE("GxScene::render");

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
      throw;
    }
}

void GxScene::fullRender()
{
GVX_TRACE("GxScene::fullRender");

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
GVX_TRACE("GxScene::undraw");
  itsUndrawNode->undraw(*itsCanvas);
  itsCanvas->flushOutput();
}

void GxScene::clearscreen()
{
GVX_TRACE("GxScene::clearscreen");
  itsCanvas->clearColorBuffer();
  setDrawable(Nub::Ref<GxNode>(GxEmptyNode::make()));
  itsUndrawNode = Nub::Ref<GxNode>(GxEmptyNode::make());
  isItVisible = false;
}

void GxScene::fullClearscreen()
{
GVX_TRACE("GxScene::fullClearscreen");
  clearscreen();
  itsCanvas->flushOutput();
}

void GxScene::setVisibility(bool val)
{
GVX_TRACE("GxScene::setVisibility");
  isItVisible = val;
  if ( !isItVisible )
    {
      fullClearscreen();
    }
}

void GxScene::setCamera(const Nub::Ref<GxCamera>& cam)
{
GVX_TRACE("GxScene::setCamera");
  itsCamera->sigNodeChanged.disconnect(slotNodeChanged);

  itsCamera = cam;

  itsCamera->reshape(*itsCanvas, itsWidth, itsHeight);

  itsCamera->sigNodeChanged.connect(slotNodeChanged);

  fullRender();
}

void GxScene::setDrawable(const Nub::Ref<GxNode>& node)
{
GVX_TRACE("GxScene::setDrawable");
  itsDrawNode->sigNodeChanged.disconnect(slotNodeChanged);

  itsDrawNode = node;

  itsDrawNode->sigNodeChanged.connect(slotNodeChanged);
}

void GxScene::flushChanges()
{
GVX_TRACE("GxScene::flushChanges");
  if (isItRefreshing && !isItRefreshed)
    {
      itsCamera->reshape(*itsCanvas, itsWidth, itsHeight);
      fullRender();
    }
}

void GxScene::onNodeChange()
{
GVX_TRACE("GxScene::onNodeChange");
  isItRefreshed = false;
  flushChanges();
}

void GxScene::reshape(int width, int height)
{
GVX_TRACE("GxScene::reshape");
  itsWidth = width;
  itsHeight = height;
  itsCamera->reshape(*itsCanvas, itsWidth, itsHeight);
}

void GxScene::animate(unsigned int framesPerSecond)
{
GVX_TRACE("GxScene::animate");
  if (framesPerSecond == 0)
    {
      itsTimer.cancel();
    }
  else
    {
      itsTimer.setDelayMsec(static_cast<unsigned int>(1000.0/framesPerSecond));
      itsTimer.schedule(itsScheduler);
    }
}

static const char vcid_groovx_gfx_gxscene_cc_utc20050626084025[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXSCENE_CC_UTC20050626084025_DEFINED
