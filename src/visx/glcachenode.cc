///////////////////////////////////////////////////////////////////////
//
// glcachenode.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 10 16:42:39 2001
// written: Wed Nov 13 10:55:11 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLCACHENODE_CC_DEFINED
#define GLCACHENODE_CC_DEFINED

#include "visx/glcachenode.h"

#include "gfx/canvas.h"

#include "gx/rect.h"

#include "util/error.h"

#include <GL/gl.h>

#include "util/debug.h"

GLCacheNode::~GLCacheNode()
{
  glDeleteLists(itsDisplayList, 1);
}

void GLCacheNode::draw(Gfx::Canvas& canvas) const
{
  if (itsMode != Gmodes::GLCOMPILE)
    {
      child()->draw(canvas);
    }
  else
    {
      // We must explicitly check that the display list is valid,
      // since it might be invalid if the object was recently
      // constructed, for example.
      if (itsDisplayList != 0 && glIsList(itsDisplayList) == GL_TRUE)
        {
          glCallList(itsDisplayList);
          dbgEvalNL(3, itsDisplayList);
        }
      else
        {
          itsDisplayList = glGenLists(1);

          if (itsDisplayList == 0)
            {
              throw Util::Error("GrObj::newList: couldn't allocate display list");
            }

          glNewList(itsDisplayList, GL_COMPILE_AND_EXECUTE);
          child()->draw(canvas);
          glEndList();
        }
    }
}

void GLCacheNode::getBoundingCube(Gfx::Box<double>& cube,
                                  Gfx::Canvas& canvas) const
{
  child()->getBoundingCube(cube, canvas);
}

void GLCacheNode::invalidate()
{
  glDeleteLists(itsDisplayList, 1);
  itsDisplayList = 0;
}

void GLCacheNode::setMode(Gmodes::RenderMode new_mode)
{
#ifdef BROKEN_GL_DISPLAY_LISTS
  if (new_mode == Gmodes::GLCOMPILE) new_mode = Gmodes::DIRECT_RENDER;
#endif

  if (itsMode != new_mode)
    invalidate();

  itsMode = new_mode;
}

static const char vcid_glcachenode_cc[] = "$Header$";
#endif // !GLCACHENODE_CC_DEFINED
