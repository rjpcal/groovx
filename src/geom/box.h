///////////////////////////////////////////////////////////////////////
//
// box.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jul  3 15:03:23 2002
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

#ifndef BOX_H_DEFINED
#define BOX_H_DEFINED

#include "geom/span.h"
#include "geom/vec3.h"
#include "geom/rect.h"

#include "util/algo.h"

#include "util/debug.h"
DBG_REGISTER

namespace geom
{
  //  #######################################################
  //  =======================================================

  /// A 3-D rectangular box with sides parallel to the major axes.
  template <class V>
  class box
  {
  private:
    geom::span<V> xx, yy, zz;

  public:
    box() : xx(), yy(), zz() {}

    box(const geom::span<V>& x,
        const geom::span<V>& y,
        const geom::span<V>& z)
      :
      xx(x), yy(y), zz(z)
    {}

    explicit box(const geom::rect<V>& rect) :
      xx(rect.x_span()),
      yy(rect.y_span()),
      zz()
    {}

    geom::rect<V> rect() const
    {
      return geom::rect<V>(xx, yy);
    }

    V x0() const { return xx.lo; }
    V x1() const { return xx.hi; }
    V y0() const { return yy.lo; }
    V y1() const { return yy.hi; }
    V z0() const { return zz.lo; }
    V z1() const { return zz.hi; }

    vec3<V> point000() const { return geom::vec3<V>(xx.lo, yy.lo, zz.lo); }
    vec3<V> point001() const { return geom::vec3<V>(xx.lo, yy.lo, zz.hi); }
    vec3<V> point010() const { return geom::vec3<V>(xx.lo, yy.hi, zz.lo); }
    vec3<V> point011() const { return geom::vec3<V>(xx.lo, yy.hi, zz.hi); }
    vec3<V> point100() const { return geom::vec3<V>(xx.hi, yy.lo, zz.lo); }
    vec3<V> point101() const { return geom::vec3<V>(xx.hi, yy.lo, zz.hi); }
    vec3<V> point110() const { return geom::vec3<V>(xx.hi, yy.hi, zz.lo); }
    vec3<V> point111() const { return geom::vec3<V>(xx.hi, yy.hi, zz.hi); }

    V size_x() const { return xx.width(); }
    V size_y() const { return yy.width(); }
    V size_z() const { return zz.width(); }

    V center_x() const { return xx.center(); }
    V center_y() const { return yy.center(); }
    V center_z() const { return zz.center(); }

    geom::vec3<V> center() const
    { return geom::vec3<V>(center_x(), center_y(), center_z()); }

    void set_xx_yy_zz(V x0, V x1, V y0, V y1, V z0, V z1)
    {
      xx = span<V>(x0,x1);
      yy = span<V>(y0,y1);
      zz = span<V>(z0,z1);
    }

    /// Like set_xx_yy_zz(), except figure out the right min/max ordering.
    void set_any_xx_yy_zz(V x0, V x1, V y0, V y1, V z0, V z1)
    {
      xx = span<V>::from_any(x0,x1);
      yy = span<V>::from_any(y0,y1);
      zz = span<V>::from_any(z0,z1);
    }

    void set_corners(const geom::vec3<V>& p1, const geom::vec3<V>& p2)
    {
      set_any_xx_yy_zz(p1.x(), p2.x(), p1.y(), p2.y(), p1.z(), p2.z());
    }

    void size_x(V s) { xx = xx.with_width(s); }
    void size_y(V s) { yy = yy.with_width(s); }
    void size_z(V s) { zz = zz.with_width(s); }

    void scale_x(V factor) { xx = xx.scaled_by(factor); }
    void scale_y(V factor) { yy = yy.scaled_by(factor); }
    void scale_z(V factor) { zz = zz.scaled_by(factor); }

    void scale(const geom::vec3<V>& factors)
    {
      scale_x(factors.x());
      scale_y(factors.y());
      scale_z(factors.z());
    }

    void scale(V factor)
    {
      scale_x(factor);
      scale_y(factor);
      scale_z(factor);
    }

    void translate(const geom::vec3<V>& dist)
    {
      xx = xx.shifted_by(dist.x());
      yy = yy.shifted_by(dist.y());
      zz = zz.shifted_by(dist.z());
    }

    void set_center(const geom::vec3<V>& point)
    {
      geom::vec3<V> diff = point - center();
      translate(diff);
    }

    bool is_void() const
    {
      return (xx.is_void() || yy.is_void() || zz.is_void());
    }

    box<V> union_with(const box<V>& other) const
    {
      return box<V>(xx.union_with(other.xx),
                    yy.union_with(other.yy),
                    zz.union_with(other.zz));
    }

    void merge(const geom::vec2<V>& p)
    {
      xx = xx.including(p.x());
      yy = yy.including(p.y());
    }

    void merge(const geom::vec3<V>& p)
    {
      xx = xx.including(p.x());
      yy = yy.including(p.y());
      zz = zz.including(p.z());
    }

    void merge(const geom::rect<V>& r)
    {
      xx = xx.union_with(r.x_span());
      yy = yy.union_with(r.y_span());
    }

    void debug_dump() const
    {
      dbg_eval(0, xx.lo); dbg_eval_nl(0, xx.hi);
      dbg_eval(0, yy.lo); dbg_eval_nl(0, yy.hi);
      dbg_eval(0, zz.lo); dbg_eval_nl(0, zz.hi);
    }
  };

} // end namespace geom

static const char vcid_box_h[] = "$Id$ $URL$";
#endif // !BOX_H_DEFINED
