///////////////////////////////////////////////////////////////////////
//
// bitmapcachenode.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:22:10 2001
// written: Wed Nov 13 11:03:34 2002
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
#include "gx/imgfile.h"
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

  if ( !itsCacheFilename.is_empty() )
    {
      itsBmapData.reset(new Gfx::BmapData());
      ImgFile::load(fullCacheFilename().c_str(), *itsBmapData);
      return;
    }

  Gfx::Rect<double> bmapbox = child()->getBoundingBox(canvas);

  Gfx::Rect<int> screen_rect = canvas.screenFromWorld(bmapbox);

  {
    Gfx::AttribSaver saver(canvas);

    canvas.drawOnFrontBuffer();

    canvas.clearColorBuffer(screen_rect);

    child()->draw(canvas);
  }

  itsBmapData.reset(new Gfx::BmapData());
  canvas.grabPixels(screen_rect, *itsBmapData);

  if (Gmodes::X11_BITMAP_CACHE == itsMode)
    {
      itsBmapData->flipContrast();
    }
}

void BitmapCacheNode::setMode(Gmodes::RenderMode new_mode)
{
DOTRACE("BitmapCacheNode::setMode");

#ifdef BROKEN_GL_DISPLAY_LISTS
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
      ImgFile::save(fullCacheFilename().c_str(), *itsBmapData);
    }
}

void BitmapCacheNode::draw(Gfx::Canvas& canvas) const
{
DOTRACE("BitmapCacheNode::draw");
  dbgEvalNL(3, itsMode);

  if (itsMode != Gmodes::GL_BITMAP_CACHE &&
      itsMode != Gmodes::X11_BITMAP_CACHE)
    {
      child()->draw(canvas);
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

void BitmapCacheNode::getBoundingCube(Gfx::Box<double>& cube,
                                      Gfx::Canvas& canvas) const
{
DOTRACE("BitmapCacheNode::getBoundingCube");

  if (itsMode != Gmodes::GL_BITMAP_CACHE &&
      itsMode != Gmodes::X11_BITMAP_CACHE)
    {
      child()->getBoundingCube(cube, canvas);
      return;
    }

  // else
  recacheBitmap(canvas);
  Assert(itsBmapData.get() != 0);

  Gfx::Vec2<int> bottom_left = canvas.screenFromWorld(Gfx::Vec2<double>());
  Gfx::Vec2<int> top_right = bottom_left + (itsBmapData->size());

  Gfx::Rect<double> bbox;

  bbox.setBottomLeft(Gfx::Vec2<double>());

  bbox.setTopRight(canvas.worldFromScreen(top_right));

  cube.unionize(bbox);
}

static const char vcid_bitmapcachenode_cc[] = "$Header$";
#endif // !BITMAPCACHENODE_CC_DEFINED
