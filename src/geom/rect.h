///////////////////////////////////////////////////////////////////////
//
// rect.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jan  4 08:00:00 1999
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

#ifndef RECT_H_DEFINED
#define RECT_H_DEFINED

#include "geom/span.h"
#include "geom/vec2.h"

#include "util/algo.h"

namespace geom
{

  // ########################################################
  /// geom::rect represents rectangles in two-dimensional space.

  template<class V>
  class rect
  {
  public:
    // Creators
    rect() : xx(), yy() {} // everything inits to zero

    rect(const geom::vec2<V>& p1, const geom::vec2<V>& p2)
    { set_corners(p1, p2); }

    rect(const rect<V>& i) : xx(i.xx), yy(i.yy) {}

    rect<V>& operator=(const rect<V>& i)
    { xx = i.xx; yy = i.yy; return *this; }

    /// Factory for building rect's with left/top/right/bottom arguments.
    static rect<V> ltrb(V l, V t, V r, V b)
    {
      return rect<V>().set_ltrb(l, t, r, b);
    }

    /// Factory for building rect's with left/bottom/width/height arguments.
    static rect<V> lbwh(V l, V b, V w, V h)
    {
      return rect<V>().set_lbwh(l, b, w, h);
    }

    // Accessors
    void get_ltrb(V& L, V& T, V& R, V& B) const
    { L = xx.lo; T = yy.hi; R = xx.hi; B = yy.lo; }
    void get_lrbt(V& L, V& R, V& B, V& T) const
    { L = xx.lo; R = xx.hi; B = yy.lo; T = yy.hi; }

    geom::vec2<V> bottom_left() const
    { return geom::vec2<V>(xx.lo, yy.lo); }

    geom::vec2<V> bottom_right() const
    { return geom::vec2<V>(xx.hi, yy.lo); }

    geom::vec2<V> top_left() const
    { return geom::vec2<V>(xx.lo, yy.hi); }

    geom::vec2<V> top_right() const
    { return geom::vec2<V>(xx.hi, yy.hi); }

    V width() const { return (xx.hi-xx.lo); }
    V height() const { return (yy.hi-yy.lo); }

    geom::vec2<V> size() const { return geom::vec2<V>(width(), height()); }

    V aspect() const { return width()/height(); }

    geom::vec2<V> center() const { return geom::vec2<V>(center_x(), center_y()); }

    V center_x() const { return (xx.hi+xx.lo)/V(2); }
    V center_y() const { return (yy.hi+yy.lo)/V(2); }

    const V& left() const { return xx.lo; }
    const V& right() const { return xx.hi; }
    const V& bottom() const { return yy.lo; }
    const V& top() const { return yy.hi; }

    bool contains(const geom::vec2<V>& pt) const
    { return pt.x()>=xx.lo && pt.x()<=xx.hi && pt.y()>=yy.lo && pt.y()<=yy.hi; }

    //
    // Manipulators
    //

    /// Set four corners from x-left/y-top/x-right/y-bottom values.
    rect<V>& set_ltrb(V L, V T, V R, V B)
    { xx.lo = L; yy.hi = T; xx.hi = R; yy.lo = B; return *this; }

    /// Set four corners from x-left/x-right/y-bottom/y-top values.
    rect<V>& set_lrbt(V L, V R, V B, V T)
    { xx.lo = L; xx.hi = R; yy.lo = B; yy.hi = T; return *this; }

    /// Set four corners from x-left/y-bottom/x-width/y-height values.
    rect<V>& set_lbwh(V x, V y, V w, V h)
    {
      xx.lo = x;
      yy.lo = y;
      xx.hi = xx.lo+rutz::abs(w);
      yy.hi = yy.lo+rutz::abs(h);
      return *this;
    }

    /// Set four corners from lower-left corner and width+height values.
    rect<V>& set_lbwh(const geom::vec2<V>& xy, const geom::vec2<V>& wh)
    {
      xx.lo = xy.x();
      yy.lo = xy.y();
      xx.hi = xx.lo+rutz::abs(wh.x());
      yy.hi = yy.lo+rutz::abs(wh.y());
      return *this;
    }

    /// Set four corners from positions of two diagonally-opposed corners.
    rect<V>& set_corners(const geom::vec2<V>& p1, const geom::vec2<V>& p2)
    {
      xx.lo = rutz::min(p1.x(), p2.x());
      xx.hi = rutz::max(p1.x(), p2.x());
      yy.lo = rutz::min(p1.y(), p2.y());
      yy.hi = rutz::max(p1.y(), p2.y());
      return *this;
    }

    rect<V>& set_bottom_left(const geom::vec2<V>& point)
    { xx.lo = point.x(); yy.lo = point.y(); return *this; }

    rect<V>& set_bottom_right(const geom::vec2<V>& point)
    { xx.hi = point.x(); yy.lo = point.y(); return *this; }

    rect<V>& set_top_left(const geom::vec2<V>& point)
    { xx.lo = point.x(); yy.hi = point.y(); return *this; }

    rect<V>& set_top_right(const geom::vec2<V>& point)
    { xx.hi = point.x(); yy.hi = point.y(); return *this; }

    void incr_width(V w)
    {
      V half = V(0.5*w);
      xx.lo -= half;
      xx.hi += (w-half); // in case V(0.5*h) != 0.5*h (e.g. integer math)
    }

    void incr_height(V h)
    {
      V half = V(0.5*h);
      yy.lo -= half;
      yy.hi += (h-half); // in case V(0.5*h) != 0.5*h (e.g. integer math)
    }

    void set_width(V w)  { incr_width(w - width()); }
    void set_height(V h) { incr_height(h - height()); }

    void scale_x(V factor) { set_width(width() * factor); }
    void scale_y(V factor) { set_height(height() * factor); }

    void translate(const geom::vec2<V>& dist)
    {
      xx.lo += dist.x(); xx.hi += dist.x();
      yy.lo += dist.y(); yy.hi += dist.y();
    }

    void scale(const geom::vec2<V>& factors)
    {
      scale_x(factors.x());
      scale_y(factors.y());
    }

    void set_center(const geom::vec2<V>& point)
    {
      geom::vec2<V> diff = point - center();
      translate(diff);
    }

    bool is_void() const { return (yy.hi <= yy.lo) || (xx.hi <= xx.lo); }

    void unionize(const rect<V>& other)
    {
      if (!other.is_void())
        {
          if (this->is_void())
            {
              operator=(other);
            }
          else
            {
              xx.lo = rutz::min(xx.lo, other.xx.lo);
              xx.hi = rutz::max(xx.hi, other.xx.hi);
              yy.lo = rutz::min(yy.lo, other.yy.lo);
              yy.hi = rutz::max(yy.hi, other.yy.hi);
            }
        }
    }

  private:
    // Data members
    geom::span<V> xx;
    geom::span<V> yy;
  };

} // end namespace geom

static const char vcid_rect_h[] = "$Header$";
#endif // !RECT_H_DEFINED
