///////////////////////////////////////////////////////////////////////
//
// bitmapcachenode.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:22:10 2001
// written: Mon Jan 21 11:56:19 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPCACHENODE_CC_DEFINED
#define BITMAPCACHENODE_CC_DEFINED

#include "visx/bitmapcachenode.h"

#include "visx/glbmaprenderer.h"
#include "visx/xbmaprenderer.h"

#include "gfx/canvas.h"

#include "gx/bmapdata.h"
#include "gx/pbm.h"
#include "gx/rect.h"

#include "util/error.h"

#include "util/trace.h"
#include "util/debug.h"

fstring BitmapCacheNode::BITMAP_CACHE_DIR(".");

BitmapCacheNode::BitmapCacheNode(Util::SoftRef<Gnode> child) :
  Gnode(child),
  itsMode(Gmodes::DIRECT_RENDER),
  itsCacheFilename(""),
  itsBmapRenderer(0),
  itsBmapData(0)
{
DOTRACE("BitmapCacheNode::BitmapCacheNode");
}

BitmapCacheNode::~BitmapCacheNode()
{
DOTRACE("BitmapCacheNode::~BitmapCacheNode");
}

void BitmapCacheNode::setCacheFilename(const fstring& name)
{
DOTRACE("BitmapCacheNode::setCacheFilename");
  itsCacheFilename = name;
  itsBmapRenderer.reset( 0 );
  itsBmapData.reset( 0 );
}

void BitmapCacheNode::invalidate()
{
DOTRACE("BitmapCacheNode::invalidate");
  itsBmapRenderer.reset( 0 );
  itsBmapData.reset( 0 );
  itsCacheFilename = "";
}

void BitmapCacheNode::recacheBitmap(Gfx::Canvas& canvas) const
{
DOTRACE("BitmapCacheNode::recacheBitmap");

  if (itsBmapData.get() != 0 && itsBmapRenderer.get() != 0)
    return;

  switch (itsMode)
    {
    case Gmodes::GL_BITMAP_CACHE:
      itsBmapRenderer.reset(new GLBmapRenderer());
      break;

    case Gmodes::X11_BITMAP_CACHE:
      itsBmapRenderer.reset(new XBmapRenderer());
      break;
    }

  Assert(itsBmapRenderer.get() != 0);

  if ( !itsCacheFilename.empty() )
    {
      itsBmapData.reset(new Gfx::BmapData());
      Pbm::load(fullCacheFilename().c_str(), *itsBmapData);
      return;
    }

  Gfx::Rect<double> bmapbox = child()->gnodeBoundingBox(canvas);

  Gfx::Rect<int> screen_rect = canvas.screenFromWorld(bmapbox);

  {
    Gfx::Canvas::AttribSaver saver(canvas);

    canvas.drawOnFrontBuffer();

    canvas.clearColorBuffer(screen_rect);

    child()->gnodeDraw(canvas);
  }

  itsBmapData.reset(new Gfx::BmapData());
  canvas.grabPixels(screen_rect, *itsBmapData);

  if (Gmodes::X11_BITMAP_CACHE == itsMode)
    {
      itsBmapData->flipVertical();
      itsBmapData->flipContrast();
    }
}

void BitmapCacheNode::setMode(Gmodes::RenderMode new_mode)
{
DOTRACE("BitmapCacheNode::setMode");

#ifdef I686
  // display lists don't work at present with i686/linux/mesa
  if (new_mode == Gmodes::GLCOMPILE) new_mode = Gmodes::DIRECT_RENDER;
#endif

  if (new_mode != itsMode)
    invalidate();

  itsMode = new_mode;
}

void BitmapCacheNode::saveBitmapCache(Gfx::Canvas& /*canvas*/,
                                      const char* filename) const
{
  if (itsBmapData.get() != 0)
    {
      itsCacheFilename = filename;
      Pbm::save(fullCacheFilename().c_str(), *itsBmapData);
    }
}

void BitmapCacheNode::gnodeDraw(Gfx::Canvas& canvas) const
{
DOTRACE("BitmapCacheNode::gnodeDraw");
  DebugEvalNL(itsMode);

  if (itsMode != Gmodes::GL_BITMAP_CACHE &&
      itsMode != Gmodes::X11_BITMAP_CACHE)
    {
      child()->gnodeDraw(canvas);
    }
  else
    {
      recacheBitmap(canvas);
      Assert(itsBmapRenderer.get() != 0 && itsBmapData.get() != 0);
      itsBmapRenderer->doRender(canvas,
                                *itsBmapData,
                                Gfx::Vec2<double>(), // position = (0,0)
                                Gfx::Vec2<double>(1.0, 1.0)); // zoom = (1,1)
    }
}

Gfx::Rect<double> BitmapCacheNode::gnodeBoundingBox(Gfx::Canvas& canvas) const
{
DOTRACE("BitmapCacheNode::gnodeBoundingBox");

  if (itsMode != Gmodes::GL_BITMAP_CACHE &&
      itsMode != Gmodes::X11_BITMAP_CACHE)
    {
      return child()->gnodeBoundingBox(canvas);
    }

  // else
  recacheBitmap(canvas);
  Assert(itsBmapData.get() != 0);

  Gfx::Vec2<int> bottom_left = canvas.screenFromWorld(Gfx::Vec2<double>());
  Gfx::Vec2<int> top_right = bottom_left + (itsBmapData->extent());

  Gfx::Rect<double> bbox;

  bbox.setBottomLeft(Gfx::Vec2<double>());

  bbox.setTopRight(canvas.worldFromScreen(top_right));

  return bbox;
}

static const char vcid_bitmapcachenode_cc[] = "$Header$";
#endif // !BITMAPCACHENODE_CC_DEFINED
