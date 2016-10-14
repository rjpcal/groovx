/** @file gfx/bbox.h compute a shrink-wrapped bounding box around a
    GxNode hierarchy */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov 19 12:37:16 2002
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

#ifndef GROOVX_GFX_BBOX_H_UTC20050626084025_DEFINED
#define GROOVX_GFX_BBOX_H_UTC20050626084025_DEFINED

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

  void translate(const geom::vec3<double>& v);
  void scale(const geom::vec3<double>& v);
  void transform(const geom::txform& m);

  void vertex2(const geom::vec2<double>& v);
  void vertex3(const geom::vec3<double>& v);

  void drawRect(const geom::rect<double>& rect);

  void drawBox(const geom::box<double>& box);

  void drawScreenRect(const geom::vec3<double>& lower_left,
                      const geom::vec2<int>& size,
                      const geom::vec2<double>& zoom);

  void drawScreenRect(const geom::vec3<double>& lower_left,
                      const geom::rect<int>& screen_rect);

  geom::box<double> cube() const;
  geom::rect<double> rect() const;

private:
  Bbox& operator=(const Bbox&);

  struct Impl;
  Impl* const rep;
};

#endif // !GROOVX_GFX_BBOX_H_UTC20050626084025_DEFINED
