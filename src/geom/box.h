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
    V xx0, xx1, yy0, yy1, zz0, zz1;

  public:
    box() :
      // init all to zero
      xx0(), xx1(), yy0(), yy1(), zz0(), zz1()
    {}

    box(const geom::rect<V>& rect) :
      xx0(rect.left()), xx1(rect.right()),
      yy0(rect.bottom()), yy1(rect.top()),
      zz0(), zz1()
    {}

    geom::rect<V> rect() const
    {
      geom::rect<V> result;
      result.set_lrbt(xx0, xx1, yy0, yy1);
      return result;
    }

    V x0() const { return xx0; }
    V x1() const { return xx1; }
    V y0() const { return yy0; }
    V y1() const { return yy1; }
    V z0() const { return zz0; }
    V z1() const { return zz1; }

    vec3<V> point000() const { return geom::vec3<V>(xx0, yy0, zz0); }
    vec3<V> point001() const { return geom::vec3<V>(xx0, yy0, zz1); }
    vec3<V> point010() const { return geom::vec3<V>(xx0, yy1, zz0); }
    vec3<V> point011() const { return geom::vec3<V>(xx0, yy1, zz1); }
    vec3<V> point100() const { return geom::vec3<V>(xx1, yy0, zz0); }
    vec3<V> point101() const { return geom::vec3<V>(xx1, yy0, zz1); }
    vec3<V> point110() const { return geom::vec3<V>(xx1, yy1, zz0); }
    vec3<V> point111() const { return geom::vec3<V>(xx1, yy1, zz1); }

    V size_x() const { return xx1 - xx0; }
    V size_y() const { return yy1 - yy0; }
    V size_z() const { return zz1 - zz0; }

    V center_x() const { return (xx0+xx1)/V(2); }
    V center_y() const { return (yy0+yy1)/V(2); }
    V center_z() const { return (zz0+zz1)/V(2); }

    geom::vec3<V> center() const
    { return geom::vec3<V>(center_x(), center_y(), center_z()); }

    void set000(const vec3<V>& pt) { xx0=pt.x(); yy0=pt.y(); zz0=pt.z(); }
    void set001(const vec3<V>& pt) { xx0=pt.x(); yy0=pt.y(); zz1=pt.z(); }
    void set010(const vec3<V>& pt) { xx0=pt.x(); yy1=pt.y(); zz0=pt.z(); }
    void set011(const vec3<V>& pt) { xx0=pt.x(); yy1=pt.y(); zz1=pt.z(); }
    void set100(const vec3<V>& pt) { xx1=pt.x(); yy0=pt.y(); zz0=pt.z(); }
    void set101(const vec3<V>& pt) { xx1=pt.x(); yy0=pt.y(); zz1=pt.z(); }
    void set110(const vec3<V>& pt) { xx1=pt.x(); yy1=pt.y(); zz0=pt.z(); }
    void set111(const vec3<V>& pt) { xx1=pt.x(); yy1=pt.y(); zz1=pt.z(); }

    void set_xx_yy_zz(V x0, V x1, V y0, V y1, V z0, V z1)
    {
      xx0 = x0; xx1 = x1;
      yy0 = y0; yy1 = y1;
      zz0 = z0; zz1 = z1;
    }

    /// Like set_xx_yy_zz(), except figure out the right min/max ordering.
    void set_any_xx_yy_zz(V x0, V x1, V y0, V y1, V z0, V z1)
    {
      xx0 = rutz::min(x0, x1); xx1 = rutz::max(x0, x1);
      yy0 = rutz::min(y0, y1); yy1 = rutz::max(y0, y1);
      zz0 = rutz::min(z0, z1); zz1 = rutz::max(z0, z1);
    }

    void set_corners(const geom::vec3<V>& p1, const geom::vec3<V>& p2)
    {
      set_any_xx_yy_zz(p1.x(), p2.x(), p1.y(), p2.y(), p1.z(), p2.z());
    }

    void size_x(V s) { V del = s - size_x(); xx0-=0.5*del; xx1+=0.5*del; }
    void size_y(V s) { V del = s - size_y(); yy0-=0.5*del; yy1+=0.5*del; }
    void size_z(V s) { V del = s - size_z(); zz0-=0.5*del; zz1+=0.5*del; }

    void scale_x(V factor) { size_x(size_x() * factor); }
    void scale_y(V factor) { size_y(size_y() * factor); }
    void scale_z(V factor) { size_z(size_z() * factor); }

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
      xx0 += dist.x(); xx1 += dist.x();
      yy0 += dist.y(); yy1 += dist.y();
      zz0 += dist.z(); zz1 += dist.z();
    }

    void set_center(const geom::vec3<V>& point)
    {
      geom::vec3<V> diff = point - center();
      translate(diff);
    }

    bool is_void() const
    {
      return (xx0>=xx1) && (yy0>=yy1) && (zz0>=zz1);
    }

    void unionize(const box<V>& other)
    {
      if (!other.is_void())
        {
          if (this->is_void())
            {
              *this = other;
            }
          else
            {
              xx0 = rutz::min(xx0, other.xx0);
              xx1 = rutz::max(xx1, other.xx1);
              yy0 = rutz::min(yy0, other.yy0);
              yy1 = rutz::max(yy1, other.yy1);
              zz0 = rutz::min(zz0, other.zz0);
              zz1 = rutz::max(zz1, other.zz1);
            }
        }
    }

    void merge(const geom::vec2<V>& p)
    {
      if (p.x() < xx0) xx0 = p.x(); else if (p.x() > xx1) xx1 = p.x();
      if (p.y() < yy0) yy0 = p.y(); else if (p.y() > yy1) yy1 = p.y();
      if (V(0)  < zz0) zz0 = V(0);  else if (V(0)  > zz1) zz1 = V(0);
    }

    void merge(const geom::vec3<V>& p)
    {
      if (p.x() < xx0) xx0 = p.x(); else if (p.x() > xx1) xx1 = p.x();
      if (p.y() < yy0) yy0 = p.y(); else if (p.y() > yy1) yy1 = p.y();
      if (p.z() < zz0) zz0 = p.z(); else if (p.z() > zz1) zz1 = p.z();
    }

    void merge(const geom::rect<V>& r)
    {
      merge(r.bottom_left());
      merge(r.top_right());
    }

    void debug_dump() const
    {
      dbg_eval(0, xx0); dbg_eval_nl(0, xx1);
      dbg_eval(0, yy0); dbg_eval_nl(0, yy1);
      dbg_eval(0, zz0); dbg_eval_nl(0, zz1);
    }
  };

} // end namespace geom

static const char vcid_box_h[] = "$Header$";
#endif // !BOX_H_DEFINED