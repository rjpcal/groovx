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

#include "gfx/canvas.h"

#include "gx/box.h"
#include "gx/txform.h"

#include <vector>

#include "util/debug.h"
DBG_REGISTER

using namespace Gfx;

struct Gfx::Bbox::Impl
{
  Impl(Canvas& c) : canvas(c), cube(), txforms(1), first(true) {}

  Canvas& canvas;
  Box<double> cube;
  std::vector<Txform> txforms;
  bool first;

  void merge(const Vec3d& v)
  {
    if (first)
      {
        Vec3d tx = txforms.back().applyTo(v);
        cube.setCorners(tx, tx);
        first = false;
      }
    else
      cube.merge(txforms.back().applyTo(v));
  }

  void merge(const Vec2d& v)
  {
    merge(Vec3d(v.x(), v.y(), 0.0));
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
  Assert(rep->txforms.size() >= 1);
  rep->txforms.push_back(rep->txforms.back());
}

void Gfx::Bbox::pop()
{
  Assert(rep->txforms.size() > 1);
  rep->txforms.pop_back();
}

Vec2i Gfx::Bbox::screenFromWorld(const Vec2d& world_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.screenFromWorld(world_pos);
}

Vec2d Gfx::Bbox::worldFromScreen(const Vec2i& screen_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.worldFromScreen(screen_pos);
}

Rect<int> Gfx::Bbox::screenFromWorld(const Rect<double>& world_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.screenFromWorld(world_pos);
}

Rect<double> Gfx::Bbox::worldFromScreen(const Rect<int>& screen_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.worldFromScreen(screen_pos);
}

void Gfx::Bbox::translate(const Vec3d& v)
{
  rep->txforms.back().translate(v);
}

void Gfx::Bbox::scale(const Vec3d& v)
{
  rep->txforms.back().scale(v);
}

void Gfx::Bbox::transform(const Txform& m)
{
  rep->txforms.back().transform(m);

  dbgDump(2, rep->txforms.back());
}

void Gfx::Bbox::vertex2(const Vec2d& v)
{
  rep->merge(v);
}

void Gfx::Bbox::vertex3(const Vec3d& v)
{
  rep->merge(v);
}

void Gfx::Bbox::drawRect(const Rect<double>& rect)
{
  rep->merge(rect.bottomLeft());
  rep->merge(rect.bottomRight());
  rep->merge(rect.topLeft());
  rep->merge(rect.topRight());
}

void Gfx::Bbox::drawBox(const Box<double>& box)
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

Box<double> Gfx::Bbox::cube() const
{
  return rep->cube;
}

Rect<double> Gfx::Bbox::rect() const
{
  return rep->cube.rect();
}

static const char vcid_bbox_cc[] = "$Header$";
#endif // !BBOX_CC_DEFINED
