///////////////////////////////////////////////////////////////////////
//
// gxcache.cc
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 14:03:42 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXCACHE_CC_DEFINED
#define GXCACHE_CC_DEFINED

#include "gxcache.h"

#include "gfx/glcanvas.h"

#include "util/error.h"

#include "util/debug.h"
DBG_REGISTER

GxCache::GxCache(Util::SoftRef<GxNode> child) :
  GxBin(child),
  itsMode(DIRECT),
  itsDisplayList(0)
{}

GxCache::~GxCache() throw()
{
  invalidate();
}

void GxCache::draw(Gfx::Canvas& canvas) const
{
  GLCanvas* const glcanvas = dynamic_cast<GLCanvas*>(&canvas);
  if (itsMode != GLCOMPILE || glcanvas == 0)
    {
      child()->draw(canvas);
    }
  else
    {
      if (glcanvas->isList(itsDisplayList))
        {
          glcanvas->callList(itsDisplayList);
          dbgEvalNL(3, itsDisplayList);
        }
      else
        {
          itsDisplayList = glcanvas->genLists(1);

          glcanvas->newList(itsDisplayList, true);
          child()->draw(canvas);
          glcanvas->endList();
        }
    }
}

void GxCache::getBoundingCube(Gfx::Bbox& bbox) const
{
  child()->getBoundingCube(bbox);
}

void GxCache::invalidate()
{
  GLCanvas::deleteLists(itsDisplayList, 1);
  itsDisplayList = 0;
}

void GxCache::setMode(Mode new_mode)
{
#ifdef BROKEN_GL_DISPLAY_LISTS
  if (new_mode == GLCOMPILE) new_mode = DIRECT;
#endif

  if (itsMode != new_mode)
    invalidate();

  itsMode = new_mode;
}

static const char vcid_gxcache_cc[] = "$Header$";
#endif // !GXCACHE_CC_DEFINED
