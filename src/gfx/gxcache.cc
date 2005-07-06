/** @file gfx/gxcache.cc GxBin subclass that can cache its child node
    in an OpenGL display list */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 13 14:03:42 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_GFX_GXCACHE_CC_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXCACHE_CC_UTC20050626084023_DEFINED

#include "gxcache.h"

#include "gfx/glcanvas.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/error.h"

#include "rutz/debug.h"
#include "rutz/trace.h"
GVX_DBG_REGISTER

namespace
{
  const IO::VersionId GXCACHE_SVID = 1;
}

GxCache::GxCache(nub::soft_ref<GxNode> child) :
  GxBin(child),
  itsMode(DIRECT),
  itsDisplayList(0),
  itsCanvas()
{
GVX_TRACE("GxCache::GxCache");
}

GxCache::~GxCache() throw()
{
GVX_TRACE("GxCache::~GxCache");
  invalidate();
}

IO::VersionId GxCache::serialVersionId() const
{
GVX_TRACE("GxCache::serialVersionId");
  return GXCACHE_SVID;
}

void GxCache::readFrom(IO::Reader& reader)
{
GVX_TRACE("GxCache::readFrom");

  reader.ensureReadVersionId("GxCache", 1,
                             "Try Revision 1.11", SRC_POS);

  reader.readValue("mode", itsMode);
}

void GxCache::writeTo(IO::Writer& writer) const
{
GVX_TRACE("GxCache::writeTo");

  writer.writeValue("mode", itsMode);
}

void GxCache::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxCache::draw");
  GLCanvas* const glcanvas = dynamic_cast<GLCanvas*>(&canvas);
  if (itsMode != GLCOMPILE || glcanvas == 0)
    {
      child()->draw(canvas);
    }
  else
    {
      if (glcanvas == itsCanvas.get_weak() &&
          glcanvas->isList(itsDisplayList))
        {
          glcanvas->callList(itsDisplayList);
          dbg_eval_nl(3, itsDisplayList);
        }
      else
        {
          itsDisplayList = glcanvas->genLists(1);
          itsCanvas = nub::soft_ref<GLCanvas>(glcanvas);

          glcanvas->newList(itsDisplayList, true);
          child()->draw(canvas);
          glcanvas->endList();
        }
    }
}

void GxCache::getBoundingCube(Gfx::Bbox& bbox) const
{
GVX_TRACE("GxCache::getBoundingCube");
  child()->getBoundingCube(bbox);
}

void GxCache::invalidate() throw()
{
GVX_TRACE("GxCache::invalidate");
  if (itsCanvas.is_valid())
    itsCanvas->deleteLists(itsDisplayList, 1);

  // Now forget the display list and its owning canvas
  itsDisplayList = 0;
  itsCanvas = nub::soft_ref<GLCanvas>();
}

void GxCache::setMode(Mode new_mode) throw()
{
GVX_TRACE("GxCache::setMode");
#ifdef GVX_BROKEN_GL_DISPLAY_LISTS
  if (new_mode == GLCOMPILE) new_mode = DIRECT;
#endif

  if (itsMode != new_mode)
    invalidate();

  itsMode = new_mode;
}

static const char vcid_groovx_gfx_gxcache_cc_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXCACHE_CC_UTC20050626084023_DEFINED
