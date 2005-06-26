///////////////////////////////////////////////////////////////////////
//
// bbox.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Nov 19 13:49:58 2002
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

#ifndef GROOVX_GFX_BBOX_CC_UTC20050626084024_DEFINED
#define GROOVX_GFX_BBOX_CC_UTC20050626084024_DEFINED

#include "bbox.h"

#include "geom/box.h"
#include "geom/txform.h"

#include "gfx/canvas.h"

#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using geom::recti;
using geom::rectd;
using geom::txform;
using geom::vec2i;
using geom::vec2d;
using geom::vec3i;
using geom::vec3d;

struct Gfx::Bbox::Impl
{
  Impl(Canvas& c) : canvas(c), cube(), txforms(), first(true)
  {
    txforms.push_back(txform::identity());
  }

  Canvas&             canvas;
  geom::box<double>   cube;
  std::vector<txform> txforms;
  bool                first;

  vec3d screenFromWorld3(const vec3d& world_pos) const
  {
    // FIXME need to install our own modelview matrix here first...
    return canvas.screenFromWorld3(world_pos);
  }

  vec3d worldFromScreen3(const vec3d& screen_pos) const
  {
    // FIXME need to install our own modelview matrix here first...
    return canvas.worldFromScreen3(screen_pos);
  }

  void merge(const vec3d& v)
  {
    dbg_eval_nl(3, first);

    if (first)
      {
        vec3d tx = txforms.back().apply_to(v);
        cube.set_corners(tx, tx);
        first = false;
      }
    else
      cube.merge(txforms.back().apply_to(v));
  }

  void mergeRaw(const vec3d& v)
  {
    dbg_eval_nl(3, first);

    if (first)
      {
        cube.set_corners(v, v);
        first = false;
      }
    else
      cube.merge(v);
  }

  void merge(const vec2d& v)
  {
    merge(vec3d(v.x(), v.y(), 0.0));
  }
};

Gfx::Bbox::Bbox(Canvas& c) :
  rep(new Impl(c))
{
GVX_TRACE("Gfx::Bbox::Bbox");
}

Gfx::Bbox::Bbox(const Bbox& that) :
  rep(new Impl(*(that.rep)))
{
GVX_TRACE("Gfx::Bbox::Bbox(copy)");
}

Gfx::Bbox::~Bbox()
{
GVX_TRACE("Gfx::Bbox::~Bbox");
  delete rep;
}

Gfx::Bbox Gfx::Bbox::peer() const
{
GVX_TRACE("Gfx::Bbox::peer");
  return Gfx::Bbox(rep->canvas);
}

void Gfx::Bbox::push()
{
GVX_TRACE("Gfx::Bbox::push");
  GVX_ASSERT(rep->txforms.size() >= 1);
  rep->txforms.push_back(rep->txforms.back());
}

void Gfx::Bbox::pop()
{
GVX_TRACE("Gfx::Bbox::pop");
  GVX_ASSERT(rep->txforms.size() > 1);
  rep->txforms.pop_back();
}

void Gfx::Bbox::translate(const vec3d& v)
{
GVX_TRACE("Gfx::Bbox::translate");
  rep->txforms.back().translate(v);
}

void Gfx::Bbox::scale(const vec3d& v)
{
GVX_TRACE("Gfx::Bbox::scale");
  rep->txforms.back().scale(v);
}

void Gfx::Bbox::transform(const txform& m)
{
GVX_TRACE("Gfx::Bbox::transform");
  rep->txforms.back().transform(m);

  dbg_dump(2, rep->txforms.back());
}

void Gfx::Bbox::vertex2(const vec2d& v)
{
GVX_TRACE("Gfx::Bbox::vertex2");
  rep->merge(v);
}

void Gfx::Bbox::vertex3(const vec3d& v)
{
GVX_TRACE("Gfx::Bbox::vertex3");
  rep->merge(v);
}

void Gfx::Bbox::drawRect(const rectd& rect)
{
GVX_TRACE("Gfx::Bbox::drawRect");
  rep->merge(rect.bottom_left());
  rep->merge(rect.bottom_right());
  rep->merge(rect.top_left());
  rep->merge(rect.top_right());
}

void Gfx::Bbox::drawBox(const geom::box<double>& box)
{
GVX_TRACE("Gfx::Bbox::drawBox");
  rep->merge(box.point000());
  rep->merge(box.point001());
  rep->merge(box.point010());
  rep->merge(box.point011());
  rep->merge(box.point100());
  rep->merge(box.point101());
  rep->merge(box.point110());
  rep->merge(box.point111());
}

void Gfx::Bbox::drawScreenRect(const vec3d& lower_left,
                               const vec2i& size,
                               const vec2d& zoom)
{
  const recti screen_rect = recti::lbwh(vec2i::zeros(), size * zoom);

  drawScreenRect(lower_left, screen_rect);
}

void Gfx::Bbox::drawScreenRect(const vec3d& lower_left,
                               const recti& screen_rect)
{
GVX_TRACE("Gfx::Bbox::drawScreenRect");
  const vec3d o = rep->screenFromWorld3(vec3d(lower_left));

  rep->mergeRaw(rep->worldFromScreen3(o+vec2d(screen_rect.bottom_left())));
  rep->mergeRaw(rep->worldFromScreen3(o+vec2d(screen_rect.bottom_right())));
  rep->mergeRaw(rep->worldFromScreen3(o+vec2d(screen_rect.top_left())));
  rep->mergeRaw(rep->worldFromScreen3(o+vec2d(screen_rect.top_right())));
}

geom::box<double> Gfx::Bbox::cube() const
{
GVX_TRACE("Gfx::Bbox::cube");
  return rep->cube;
}

rectd Gfx::Bbox::rect() const
{
GVX_TRACE("Gfx::Bbox::rect");
  return rep->cube.rect();
}

static const char vcid_groovx_gfx_bbox_cc_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_BBOX_CC_UTC20050626084024_DEFINED
