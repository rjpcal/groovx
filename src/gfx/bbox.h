///////////////////////////////////////////////////////////////////////
//
// bbox.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Nov 19 12:37:16 2002
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

#ifndef BBOX_H_DEFINED
#define BBOX_H_DEFINED

namespace geom
{
  template <class V> class box;
  template <class V> class rect;
  template <class V> class vec2;
  template <class V> class vec3;
  class txform;
}

namespace Gfx
{
  class Bbox;
  class Canvas;
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

  geom::vec2<double> screenFromWorld2(const geom::vec2<double>& world_pos) const;
  geom::vec2<double> worldFromScreen2(const geom::vec2<int>& screen_pos) const;

  void translate(const geom::vec3<double>& v);
  void scale(const geom::vec3<double>& v);
  void transform(const geom::txform& m);

  void vertex2(const geom::vec2<double>& v);
  void vertex3(const geom::vec3<double>& v);

  void drawRect(const geom::rect<double>& rect);

  void drawBox(const geom::box<double>& box);

  void drawScreenRect(const geom::vec2<double>& lower_left,
                      const geom::vec2<int>& size,
                      const geom::vec2<double>& zoom);

  geom::box<double> cube() const;
  geom::rect<double> rect() const;

private:
  Bbox& operator=(const Bbox&);

  struct Impl;
  Impl* const rep;
};

static const char vcid_bbox_h[] = "$Header$";
#endif // !BBOX_H_DEFINED
