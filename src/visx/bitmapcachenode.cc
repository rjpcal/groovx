///////////////////////////////////////////////////////////////////////
//
// bitmapcachenode.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:22:10 2001
// written: Fri Jan 18 16:07:03 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPCACHENODE_CC_DEFINED
#define BITMAPCACHENODE_CC_DEFINED

#include "visx/bitmapcachenode.h"

#include "visx/bitmaprep.h"
#include "visx/glbmaprenderer.h"
#include "visx/xbmaprenderer.h"

#include "gfx/canvas.h"

#include "gx/rect.h"

#include "util/error.h"

#include "util/trace.h"
#include "util/debug.h"

fstring BitmapCacheNode::BITMAP_CACHE_DIR(".");

BitmapCacheNode::BitmapCacheNode(Util::SoftRef<Gnode> child) :
  Gnode(child),
  itsMode(Gmodes::DIRECT_RENDER),
  itsCacheFilename(""),
  itsBitmapRep(0)
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
  itsBitmapRep.reset( 0 );
}

void BitmapCacheNode::invalidate()
{
DOTRACE("BitmapCacheNode::invalidate");
  itsBitmapRep.reset( 0 );
  itsCacheFilename = "";
}

void BitmapCacheNode::recacheBitmap(Gfx::Canvas& canvas) const
{
DOTRACE("BitmapCacheNode::recacheBitmap");

  if (itsBitmapRep.get() != 0)
    return;

  switch (itsMode)
    {
    case Gmodes::GL_BITMAP_CACHE:
      itsBitmapRep.reset(
            new BitmapRep(shared_ptr<BmapRenderer>(new GLBmapRenderer())));
      break;

    case Gmodes::X11_BITMAP_CACHE:
      itsBitmapRep.reset(
            new BitmapRep(shared_ptr<BmapRenderer>(new XBmapRenderer())));
      break;
    }

  Assert(itsBitmapRep.get() != 0);

  if ( !itsCacheFilename.empty() )
    {
      itsBitmapRep->queuePbmFile(fullCacheFilename().c_str());
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

  itsBitmapRep->grabWorldRect(bmapbox);

  if (Gmodes::X11_BITMAP_CACHE == itsMode)
    {
      itsBitmapRep->flipVertical();
      itsBitmapRep->flipContrast();
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

void BitmapCacheNode::saveBitmapCache(Gfx::Canvas& canvas,
                                      const char* filename) const
{
  if (itsBitmapRep.get() != 0)
    {
      itsCacheFilename = filename;
      itsBitmapRep->savePbmFile(fullCacheFilename().c_str());
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
      Assert(itsBitmapRep.get() != 0);
      itsBitmapRep->render(canvas);
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
  Assert(itsBitmapRep.get() != 0);
  return itsBitmapRep->grGetBoundingBox(canvas);
}

static const char vcid_bitmapcachenode_cc[] = "$Header$";
#endif // !BITMAPCACHENODE_CC_DEFINED
