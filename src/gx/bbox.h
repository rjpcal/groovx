///////////////////////////////////////////////////////////////////////
//
// bbox.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Nov 19 12:37:16 2002
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

#ifndef BBOX_H_DEFINED
#define BBOX_H_DEFINED

namespace Gfx
{
  class Bbox;
  class Canvas;
  class Txform;
  template <class V> class Box;
  template <class V> class Rect;
  template <class V> class Vec2;
  template <class V> class Vec3;
}

/// Computes a "shrink-wrapped" bounding box around a set of graphics primitives.
class Gfx::Bbox
{
public:
  Bbox(Gfx::Canvas& c);

  Bbox(const Bbox& that);

  ~Bbox();

  Gfx::Bbox peer() const;

  void push();
  void pop();

  Gfx::Vec2<int> screenFromWorld(const Gfx::Vec2<double>& world_pos) const;

  Gfx::Vec2<double> worldFromScreen(const Gfx::Vec2<int>& screen_pos) const;

  Gfx::Rect<int> screenFromWorld(const Gfx::Rect<double>& world_pos) const;

  Gfx::Rect<double> worldFromScreen(const Gfx::Rect<int>& screen_pos) const;

  void translate(const Gfx::Vec3<double>& v);
  void scale(const Gfx::Vec3<double>& v);
  void transform(const Gfx::Txform& m);

  void vertex2(const Gfx::Vec2<double>& v);
  void vertex3(const Gfx::Vec3<double>& v);

  void drawRect(const Gfx::Rect<double>& rect);

  void drawBox(const Gfx::Box<double>& box);

  Gfx::Box<double> cube() const;
  Gfx::Rect<double> rect() const;

private:
  Bbox& operator=(const Bbox&);

  struct Impl;
  Impl* const rep;
};

static const char vcid_bbox_h[] = "$Header$";
#endif // !BBOX_H_DEFINED
