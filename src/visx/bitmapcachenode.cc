///////////////////////////////////////////////////////////////////////
//
// bitmapcachenode.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:22:10 2001
// written: Fri Aug 10 18:10:20 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPCACHENODE_CC_DEFINED
#define BITMAPCACHENODE_CC_DEFINED

#include "bitmapcachenode.h"

#include "bitmaprep.h"
#include "glbmaprenderer.h"
#include "rect.h"
#include "xbmaprenderer.h"

#include "gfx/canvas.h"

#include "util/error.h"

#include <GL/gl.h>

#include "util/debug.h"
#include "util/trace.h"

fstring BitmapCacheNode::BITMAP_CACHE_DIR(".");

BitmapCacheNode::BitmapCacheNode(shared_ptr<Gnode> child) :
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
}

bool BitmapCacheNode::recacheBitmap(Gfx::Canvas& canvas) const
{
DOTRACE("BitmapCacheNode::recacheBitmap");

  if (itsBitmapRep.get() != 0) return false;

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
      return false;
    }

  child()->gnodeUndraw(canvas);

  Rect<double> bmapbox = child()->gnodeBoundingBox(canvas);

  glPushAttrib(GL_COLOR_BUFFER_BIT);
  {
    glDrawBuffer(GL_FRONT);

    child()->gnodeDraw(canvas);

    itsBitmapRep->grabWorldRect(bmapbox);
  }
  glPopAttrib();

  if (Gmodes::X11_BITMAP_CACHE == itsMode)
    {
      itsBitmapRep->flipVertical();
      itsBitmapRep->flipContrast();
    }

  return true;
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
      bool objectDrawn = recacheBitmap(canvas);
      if (!objectDrawn)
        {
          Assert(itsBitmapRep.get() != 0);
          itsBitmapRep->render(canvas);
        }
    }
}

void BitmapCacheNode::gnodeUndraw(Gfx::Canvas& canvas) const
{
DOTRACE("BitmapCacheNode::gnodeUndraw");

  child()->gnodeUndraw(canvas);
}

Rect<double> BitmapCacheNode::gnodeBoundingBox(Gfx::Canvas& canvas) const
{
  return child()->gnodeBoundingBox(canvas);
}

static const char vcid_bitmapcachenode_cc[] = "$Header$";
#endif // !BITMAPCACHENODE_CC_DEFINED
