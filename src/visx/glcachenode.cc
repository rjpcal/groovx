///////////////////////////////////////////////////////////////////////
//
// glcachenode.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 10 16:42:39 2001
// written: Mon Aug 13 12:18:49 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLCACHENODE_CC_DEFINED
#define GLCACHENODE_CC_DEFINED

#include "glcachenode.h"

#include "gfx/canvas.h"
#include "gfx/rect.h"

#include "util/error.h"

#include <GL/gl.h>

#include "util/debug.h"

GLCacheNode::~GLCacheNode()
{
  glDeleteLists(itsDisplayList, 1);
}

void GLCacheNode::gnodeDraw(Gfx::Canvas& canvas) const
{
  canvas.throwIfError("before GLCacheNode::gnodeDraw");

  if (itsMode != Gmodes::GLCOMPILE)
    {
      child()->gnodeDraw(canvas);
    }
  else
    {
      // We must explicitly check that the display list is valid,
      // since it might be invalid if the object was recently
      // constructed, for example.
      if (itsDisplayList != 0 && glIsList(itsDisplayList) == GL_TRUE)
        {
          glCallList(itsDisplayList);
          DebugEvalNL(itsDisplayList);
        }
      else
        {
          itsDisplayList = glGenLists(1);

          if (itsDisplayList == 0)
            {
              throw Util::Error("GrObj::newList: couldn't allocate display list");
            }

          glNewList(itsDisplayList, GL_COMPILE_AND_EXECUTE);
          child()->gnodeDraw(canvas);
          glEndList();
        }
    }

  canvas.throwIfError("during GLCacheNode::gnodeDraw");
}

void GLCacheNode::gnodeUndraw(Gfx::Canvas& canvas) const
{
  canvas.throwIfError("before GLCacheNode::gnodeUndraw");

  if (itsUnMode == Gmodes::CLEAR_BOUNDING_BOX)
    {
      Gfx::Rect<double> world_pos = gnodeBoundingBox(canvas);

      Gfx::Rect<int> screen_pos = canvas.screenFromWorld(world_pos);

      canvas.clearColorBuffer(screen_pos);
    }
  else
    {
      glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
      {
        if (itsUnMode == Gmodes::SWAP_FORE_BACK)
          {
            canvas.swapForeBack();
          }

        child()->gnodeUndraw(canvas);
      }
      glPopAttrib();
    }

  canvas.throwIfError("during GLCacheNode::gnodeUndraw");
}

Gfx::Rect<double> GLCacheNode::gnodeBoundingBox(Gfx::Canvas& canvas) const
{
  return child()->gnodeBoundingBox(canvas);
}

void GLCacheNode::invalidate()
{
  glDeleteLists(itsDisplayList, 1);
  itsDisplayList = 0;
}

void GLCacheNode::setMode(Gmodes::RenderMode new_mode)
{
#ifdef I686
  // display lists don't work at present with i686/linux/mesa
  if (new_mode == Gmodes::GLCOMPILE) new_mode = Gmodes::DIRECT_RENDER;
#endif

  if (itsMode != new_mode)
    invalidate();
}

void GLCacheNode::setUnMode(Gmodes::RenderMode new_mode)
{
  itsUnMode = new_mode;
}


static const char vcid_glcachenode_cc[] = "$Header$";
#endif // !GLCACHENODE_CC_DEFINED
