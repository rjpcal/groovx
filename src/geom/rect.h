///////////////////////////////////////////////////////////////////////
//
// rect.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jan  4 08:00:00 1999
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

#ifndef RECT_H_DEFINED
#define RECT_H_DEFINED

#include "geom/span.h"
#include "geom/vec2.h"

#include "util/algo.h"
#include "util/error.h"

#include "util/debug.h"
DBG_REGISTER

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

    rect(const geom::span<V>& x, const geom::span<V>& y) : xx(x), yy(y) {}

    rect(const geom::vec2<V>& p1, const geom::vec2<V>& p2)
    { set_corners(p1, p2); }

    rect(const rect<V>& i) : xx(i.xx), yy(i.yy) {}

    template <class U>
    explicit rect(const rect<U>& other) :
      xx(other.x_span()), yy(other.y_span()) {}

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

    /// Set four corners from lower-left corner and width+height values.
    static rect<V> lbwh(const geom::vec2<V>& xy, const geom::vec2<V>& wh)
    {
      return lbwh(xy.x(), xy.y(), wh.x(), wh.y());
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

    V center_x() const { return xx.center(); }
    V center_y() const { return yy.center(); }

    const span<V>& x_span() const { return xx; }
    const span<V>& y_span() const { return yy; }

    const V& left() const { return xx.lo; }
    const V& right() const { return xx.hi; }
    const V& bottom() const { return yy.lo; }
    const V& top() const { return yy.hi; }

    template <class U>
    bool contains(const geom::vec2<U>& pt) const
    { return xx.contains(pt.x()) && yy.contains(pt.y()); }

    //
    // Manipulators
    //

    /// Set four corners from x-left/y-top/x-right/y-bottom values.
    rect<V>& set_ltrb(V L, V T, V R, V B)
    {
      if (L > R)
        throw rutz::error("invalid rect (left > right)", SRC_POS);

      if (B > T)
        throw rutz::error("invalid rect (bottom > top)", SRC_POS);

      xx = span<V>(L,R);
      yy = span<V>(B,T);
      return *this;
    }

    /// Set four corners from x-left/x-right/y-bottom/y-top values.
    rect<V>& set_lrbt(V L, V R, V B, V T)
    {
      return this->set_ltrb(L,T,R,B);
    }

    /// Set four corners from x-left/y-bottom/x-width/y-height values.
    rect<V>& set_lbwh(V x, V y, V w, V h)
    {
      xx = span<V>(x, x+rutz::abs(w));
      yy = span<V>(y, y+rutz::abs(h));
      return *this;
    }

    /// Set four corners from lower-left corner and width+height values.
    rect<V>& set_lbwh(const geom::vec2<V>& xy, const geom::vec2<V>& wh)
    {
      return set_lbwh(xy.x(), xy.y(), wh.x(), wh.y());
    }

    /// Set four corners from positions of two diagonally-opposed corners.
    rect<V>& set_corners(const geom::vec2<V>& p1, const geom::vec2<V>& p2)
    {
      xx = span<V>::from_any(p1.x(), p2.x());
      yy = span<V>::from_any(p1.y(), p2.y());
      return *this;
    }

    void incr_width(V w)  { xx = xx.incr_width(w); }
    void incr_height(V h) { yy = yy.incr_width(h); }

    void set_width(V w)  { xx = xx.with_width(w); }
    void set_height(V h) { yy = yy.with_width(h); }

    void scale_x(V factor) { xx = xx.scaled_by(factor); }
    void scale_y(V factor) { yy = yy.scaled_by(factor); }

    void translate(const geom::vec2<V>& dist)
    {
      xx = xx.shifted_by(dist.x());
      yy = yy.shifted_by(dist.y());
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

    bool is_void() const { return (xx.is_void() || yy.is_void()); }

    rect<V> union_with(const rect<V>& other) const
    {
      return rect<V>(xx.union_with(other.x_span()),
                     yy.union_with(other.y_span()));
    }

    void debug_dump() const throw()
    {
      dbg_eval(0, left());
      dbg_eval(0, right());
      dbg_eval(0, bottom());
      dbg_eval_nl(0, top());
    }

  private:
    geom::span<V> xx;
    geom::span<V> yy;
  };

  typedef rect<double> rectd;
  typedef rect<int>    recti;

} // end namespace geom

static const char vcid_rect_h[] = "$Id$ $URL$";
#endif // !RECT_H_DEFINED
