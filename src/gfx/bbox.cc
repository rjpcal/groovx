///////////////////////////////////////////////////////////////////////
//
// bbox.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Nov 19 13:49:58 2002
// commit: $Id$
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

#ifndef BBOX_CC_DEFINED
#define BBOX_CC_DEFINED

#include "bbox.h"

#include "geom/box.h"
#include "geom/txform.h"

#include "gfx/canvas.h"

#include <vector>

#include "util/debug.h"
DBG_REGISTER

using geom::vec2i;
using geom::vec2d;
using geom::vec3d;

struct Gfx::Bbox::Impl
{
  Impl(Canvas& c) : canvas(c), cube(), txforms(1), first(true) {}

  Canvas& canvas;
  geom::box<double> cube;
  std::vector<geom::txform> txforms;
  bool first;

  void merge(const vec3d& v)
  {
    if (first)
      {
        vec3d tx = txforms.back().apply_to(v);
        cube.set_corners(tx, tx);
        first = false;
      }
    else
      cube.merge(txforms.back().apply_to(v));
  }

  void merge(const vec2d& v)
  {
    merge(vec3d(v.x(), v.y(), 0.0));
  }
};

Gfx::Bbox::Bbox(Canvas& c) :
  rep(new Impl(c))
{}

Gfx::Bbox::Bbox(const Bbox& that) :
  rep(new Impl(*(that.rep)))
{}

Gfx::Bbox::~Bbox()
{
  delete rep;
}

Gfx::Bbox Gfx::Bbox::peer() const
{
  return Gfx::Bbox(rep->canvas);
}

void Gfx::Bbox::push()
{
  ASSERT(rep->txforms.size() >= 1);
  rep->txforms.push_back(rep->txforms.back());
}

void Gfx::Bbox::pop()
{
  ASSERT(rep->txforms.size() > 1);
  rep->txforms.pop_back();
}

vec2d Gfx::Bbox::screenFromWorld2(const vec2d& world_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.screenFromWorld2(world_pos);
}

vec2d Gfx::Bbox::worldFromScreen2(const vec2i& screen_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.worldFromScreen2(screen_pos);
}

geom::rect<int> Gfx::Bbox::screenFromWorldRect(const geom::rect<double>& world_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.screenFromWorldRect(world_pos);
}

geom::rect<double> Gfx::Bbox::worldFromScreenRect(const geom::rect<int>& screen_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.worldFromScreenRect(screen_pos);
}

void Gfx::Bbox::translate(const vec3d& v)
{
  rep->txforms.back().translate(v);
}

void Gfx::Bbox::scale(const vec3d& v)
{
  rep->txforms.back().scale(v);
}

void Gfx::Bbox::transform(const geom::txform& m)
{
  rep->txforms.back().transform(m);

  dbg_dump(2, rep->txforms.back());
}

void Gfx::Bbox::vertex2(const vec2d& v)
{
  rep->merge(v);
}

void Gfx::Bbox::vertex3(const vec3d& v)
{
  rep->merge(v);
}

void Gfx::Bbox::drawRect(const geom::rect<double>& rect)
{
  rep->merge(rect.bottom_left());
  rep->merge(rect.bottom_right());
  rep->merge(rect.top_left());
  rep->merge(rect.top_right());
}

void Gfx::Bbox::drawBox(const geom::box<double>& box)
{
  rep->merge(box.point000());
  rep->merge(box.point001());
  rep->merge(box.point010());
  rep->merge(box.point011());
  rep->merge(box.point100());
  rep->merge(box.point101());
  rep->merge(box.point110());
  rep->merge(box.point111());
}

geom::box<double> Gfx::Bbox::cube() const
{
  return rep->cube;
}

geom::rect<double> Gfx::Bbox::rect() const
{
  return rep->cube.rect();
}

static const char vcid_bbox_cc[] = "$Header$";
#endif // !BBOX_CC_DEFINED
