///////////////////////////////////////////////////////////////////////
//
// bbox.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 19 13:49:58 2002
// written: Tue Nov 26 19:27:37 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BBOX_CC_DEFINED
#define BBOX_CC_DEFINED

#include "bbox.h"

#include "gfx/canvas.h"

#include "gx/box.h"
#include "gx/txform.h"

#include "util/minivec.h"

#include "util/debug.h"

struct Gfx::Bbox::Impl
{
  Impl(Gfx::Canvas& c) : canvas(c), cube(), txforms(1), first(true) {}

  Gfx::Canvas& canvas;
  Gfx::Box<double> cube;
  minivec<Gfx::Txform> txforms;
  bool first;

  void merge(const Gfx::Vec3<double>& v)
  {
    if (first)
      {
        Gfx::Vec3<double> tx = txforms.back().applyTo(v);
        cube.setCorners(tx, tx);
        first = false;
      }
    else
      cube.merge(txforms.back().applyTo(v));
  }

  void merge(const Gfx::Vec2<double>& v)
  {
    merge(Gfx::Vec3<double>(v.x(), v.y(), 0.0));
  }
};

Gfx::Bbox::Bbox(Gfx::Canvas& c) :
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

Gfx::Vec2<int> Gfx::Bbox::screenFromWorld(const Gfx::Vec2<double>& world_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.screenFromWorld(world_pos);
}

Gfx::Vec2<double> Gfx::Bbox::worldFromScreen(const Gfx::Vec2<int>& screen_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.worldFromScreen(screen_pos);
}

Gfx::Rect<int> Gfx::Bbox::screenFromWorld(const Gfx::Rect<double>& world_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.screenFromWorld(world_pos);
}

Gfx::Rect<double> Gfx::Bbox::worldFromScreen(const Gfx::Rect<int>& screen_pos) const
{
  // FIXME need to install our own modelview matrix here first...
  return rep->canvas.worldFromScreen(screen_pos);
}

void Gfx::Bbox::translate(const Gfx::Vec3<double>& v)
{
  rep->txforms.back().translate(v);
}

void Gfx::Bbox::scale(const Gfx::Vec3<double>& v)
{
  rep->txforms.back().scale(v);
}

void Gfx::Bbox::transform(const Gfx::Txform& m)
{
  rep->txforms.back().transform(m);

  dbgDump(2, rep->txforms.back());
}

void Gfx::Bbox::vertex2(const Gfx::Vec2<double>& v)
{
  rep->merge(v);
}

void Gfx::Bbox::vertex3(const Gfx::Vec3<double>& v)
{
  rep->merge(v);
}

void Gfx::Bbox::drawRect(const Gfx::Rect<double>& rect)
{
  rep->merge(rect.bottomLeft());
  rep->merge(rect.bottomRight());
  rep->merge(rect.topLeft());
  rep->merge(rect.topRight());
}

void Gfx::Bbox::drawBox(const Gfx::Box<double>& box)
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

Gfx::Box<double> Gfx::Bbox::cube() const
{
  return rep->cube;
}

Gfx::Rect<double> Gfx::Bbox::rect() const
{
  return rep->cube.rect();
}

static const char vcid_bbox_cc[] = "$Header$";
#endif // !BBOX_CC_DEFINED
