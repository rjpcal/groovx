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
    rect() : ll(), tt(), rr(), bb() {} // everything inits to zero

    rect(const geom::vec2<V>& p1, const geom::vec2<V>& p2)
    { set_corners(p1, p2); }

    rect(const rect<V>& i) : ll(i.ll), tt(i.tt), rr(i.rr), bb(i.bb) {}

    rect<V>& operator=(const rect<V>& i)
    { ll=i.ll; tt=i.tt; rr=i.rr; bb=i.bb; return *this; }

    // Accessors
    void get_ltrb(V& L, V& T, V& R, V& B) const
    { L = ll; T = tt; R = rr; B = bb; }
    void get_lrbt(V& L, V& R, V& B, V& T) const
    { L = ll; R = rr; B = bb; T = tt; }

    geom::vec2<V> bottom_left() const
    { return geom::vec2<V>(ll, bb); }

    geom::vec2<V> bottom_right() const
    { return geom::vec2<V>(rr, bb); }

    geom::vec2<V> top_left() const
    { return geom::vec2<V>(ll, tt); }

    geom::vec2<V> top_right() const
    { return geom::vec2<V>(rr, tt); }

    V width() const { return (rr-ll); }
    V height() const { return (tt-bb); }

    geom::vec2<V> size() const { return geom::vec2<V>(width(), height()); }

    V aspect() const { return width()/height(); }

    geom::vec2<V> center() const { return geom::vec2<V>(center_x(), center_y()); }

    V center_x() const { return (rr+ll)/V(2); }
    V center_y() const { return (tt+bb)/V(2); }

    V& left() { return ll; }
    V& right() { return rr; }
    V& bottom() { return bb; }
    V& top() { return tt; }

    const V& left() const { return ll; }
    const V& right() const { return rr; }
    const V& bottom() const { return bb; }
    const V& top() const { return tt; }

    bool contains(const geom::vec2<V>& pt) const
    { return pt.x()>=ll && pt.x()<=rr && pt.y()>=bb && pt.y()<=tt; }

    //
    // Manipulators
    //

    /// Set four corners from x-left/y-top/x-right/y-bottom values.
    rect<V>& set_ltrb(V L, V T, V R, V B)
    { ll = L; tt = T; rr = R; bb = B; return *this; }

    /// Set four corners from x-left/x-right/y-bottom/y-top values.
    rect<V>& set_lrbt(V L, V R, V B, V T)
    { ll = L; rr = R; bb = B; tt = T; return *this; }

    /// Set four corners from x-left/y-bottom/x-width/y-height values.
    rect<V>& set_lbwh(V x, V y, V w, V h)
    {
      ll = x;
      bb = y;
      rr = ll+rutz::abs(w);
      tt = bb+rutz::abs(h);
      return *this;
    }

    /// Set four corners from lower-left corner and width+height values.
    rect<V>& set_lbwh(const geom::vec2<V>& xy, const geom::vec2<V>& wh)
    {
      ll = xy.x();
      bb = xy.y();
      rr = ll+rutz::abs(wh.x());
      tt = bb+rutz::abs(wh.y());
      return *this;
    }

    /// Set four corners from positions of two diagonally-opposed corners.
    rect<V>& set_corners(const geom::vec2<V>& p1, const geom::vec2<V>& p2)
    {
      ll = rutz::min(p1.x(), p2.x());
      rr = rutz::max(p1.x(), p2.x());
      bb = rutz::min(p1.y(), p2.y());
      tt = rutz::max(p1.y(), p2.y());
      return *this;
    }

    rect<V>& set_bottom_left(const geom::vec2<V>& point)
    { ll = point.x(); bb = point.y(); return *this; }

    rect<V>& set_bottom_right(const geom::vec2<V>& point)
    { rr = point.x(); bb = point.y(); return *this; }

    rect<V>& set_top_left(const geom::vec2<V>& point)
    { ll = point.x(); tt = point.y(); return *this; }

    rect<V>& set_top_right(const geom::vec2<V>& point)
    { rr = point.x(); tt = point.y(); return *this; }

    void incr_width(V w)
    {
      V half = V(0.5*w);
      ll -= half;
      rr += (w-half); // in case V(0.5*h) != 0.5*h (e.g. integer math)
    }

    void incr_height(V h)
    {
      V half = V(0.5*h);
      bb -= half;
      tt += (h-half); // in case V(0.5*h) != 0.5*h (e.g. integer math)
    }

    void set_width(V w)  { incr_width(w - width()); }
    void set_height(V h) { incr_height(h - height()); }

    void scale_x(V factor) { set_width(width() * factor); }
    void scale_y(V factor) { set_height(height() * factor); }

    void translate(const geom::vec2<V>& dist)
    {
      ll += dist.x(); rr += dist.x();
      bb += dist.y(); tt += dist.y();
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

    bool is_void() const { return (tt <= bb) || (rr <= ll); }

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
              ll = rutz::min(ll, other.ll);
              rr = rutz::max(rr, other.rr);
              bb = rutz::min(bb, other.bb);
              tt = rutz::max(tt, other.tt);
            }
        }
    }

  private:
    // Data members
    V ll, tt, rr, bb;
  };

  /// Factory for building rect's with left/top/right/bottom arguments.
  template <class V>
  rect<V> rect_ltrb(V l, V t, V r, V b)
  {
    return rect<V>().set_ltrb(l, t, r, b);
  }

  /// Factory for building rect's with left/bottom/width/height arguments.
  template <class V>
  rect<V> rect_lbwh(V l, V b, V w, V h)
  {
    return rect<V>().set_lbwh(l, b, w, h);
  }

} // end namespace geom

static const char vcid_rect_h[] = "$Header$";
#endif // !RECT_H_DEFINED
