///////////////////////////////////////////////////////////////////////
//
// rect.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Wed Mar 19 17:55:56 2003
// $Id$
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

#ifndef RECT_H_DEFINED
#define RECT_H_DEFINED

#include "gx/vec2.h"

#include "util/algo.h"

namespace Gfx
{
  template <class V> class Rect;

  // These are utility classes that just expose a constructor that
  // takes arguments in the order specified by the mnemonic
  template <class V> class RectLTRB;
  template <class V> class RectLBWH;

// ####################################################################

/// Rect represents rectangles in two-dimensional space.

template<class V>
class Rect
{
public:
  // Creators
  Rect() : ll(), tt(), rr(), bb() {} // everything inits to zero

  Rect(const Gfx::Vec2<V>& p1, const Gfx::Vec2<V>& p2)
    { setCorners(p1, p2); }

  Rect(const Rect<V>& i) : ll(i.ll), tt(i.tt), rr(i.rr), bb(i.bb) {}

  Rect<V>& operator=(const Rect<V>& i)
    { ll=i.ll; tt=i.tt; rr=i.rr; bb=i.bb; return *this; }

  // Accessors
  void getRectLTRB(V& L, V& T, V& R, V& B) const
    { L = ll; T = tt; R = rr; B = bb; }
  void getRectLRBT(V& L, V& R, V& B, V& T) const
    { L = ll; R = rr; B = bb; T = tt; }

  Gfx::Vec2<V> bottomLeft() const
    { return Gfx::Vec2<V>(ll, bb); }

  Gfx::Vec2<V> bottomRight() const
    { return Gfx::Vec2<V>(rr, bb); }

  Gfx::Vec2<V> topLeft() const
    { return Gfx::Vec2<V>(ll, tt); }

  Gfx::Vec2<V> topRight() const
    { return Gfx::Vec2<V>(rr, tt); }

  V width() const { return (rr-ll); }
  V height() const { return (tt-bb); }

  Gfx::Vec2<V> size() const { return Gfx::Vec2<V>(width(), height()); }

  V aspect() const { return width()/height(); }

  Gfx::Vec2<V> center() const { return Gfx::Vec2<V>(centerX(), centerY()); }

  V centerX() const { return (rr+ll)/V(2); }
  V centerY() const { return (tt+bb)/V(2); }

  // Manipulators
  void setRectLTRB(V L, V T, V R, V B)
    { ll = L; tt = T; rr = R; bb = B; }
  void setRectLRBT(V L, V R, V B, V T)
    { ll = L; rr = R; bb = B; tt = T; }

  void setRectXYWH(V x, V y, V w, V h)
  {
    ll = x;
    bb = y;
    rr = ll+Util::abs(w);
    tt = bb+Util::abs(h);
  }

  void setCorners(const Gfx::Vec2<V>& p1, const Gfx::Vec2<V>& p2)
  {
    ll = Util::min(p1.x(), p2.x());
    rr = Util::max(p1.x(), p2.x());
    bb = Util::min(p1.y(), p2.y());
    tt = Util::max(p1.y(), p2.y());
  }

  void setBottomLeft(const Gfx::Vec2<V>& point)
    { ll = point.x(); bb = point.y(); }

  void setBottomRight(const Gfx::Vec2<V>& point)
    { rr = point.x(); bb = point.y(); }

  void setTopLeft(const Gfx::Vec2<V>& point)
    { ll = point.x(); tt = point.y(); }

  void setTopRight(const Gfx::Vec2<V>& point)
    { rr = point.x(); tt = point.y(); }

  void addWidth(V w)
  {
    V half = V(0.5*w);
    ll -= half;
    rr += (w-half); // in case V(0.5*h) != 0.5*h (e.g. integer math)
  }

  void addHeight(V h)
  {
    V half = V(0.5*h);
    bb -= half;
    tt += (h-half); // in case V(0.5*h) != 0.5*h (e.g. integer math)
  }

  void setWidth(V w)  { addWidth(w - width()); }
  void setHeight(V h) { addHeight(h - height()); }

  void scaleX(V factor) { setWidth(width() * factor); }
  void scaleY(V factor) { setHeight(height() * factor); }

  void translate(const Gfx::Vec2<V>& dist)
  {
    ll += dist.x(); rr += dist.x();
    bb += dist.y(); tt += dist.y();
  }

  void scale(const Gfx::Vec2<V>& factors)
  {
    scaleX(factors.x());
    scaleY(factors.y());
  }

  void setCenter(const Gfx::Vec2<V>& point)
  {
    Gfx::Vec2<V> diff = point - center();
    translate(diff);
  }

  bool isVoid() const { return (tt <= bb) || (rr <= ll); }

  void unionize(const Rect<V>& other)
  {
    if (!other.isVoid())
      {
        if (this->isVoid())
          {
            operator=(other);
          }
        else
          {
            ll = Util::min(ll, other.ll);
            rr = Util::max(rr, other.rr);
            bb = Util::min(bb, other.bb);
            tt = Util::max(tt, other.tt);
          }
      }
  }

  V& left() { return ll; }
  V& right() { return rr; }
  V& bottom() { return bb; }
  V& top() { return tt; }

  const V& left() const { return ll; }
  const V& right() const { return rr; }
  const V& bottom() const { return bb; }
  const V& top() const { return tt; }

private:
  // Data members
  V ll, tt, rr, bb;
};


/// Helper for building Rect's with left/top/right/bottom arguments.
template <class V>
class RectLTRB : public Rect<V>
{
public:
  RectLTRB(V l, V t, V r, V b) :
    Rect<V>()
  {
    setRectLTRB(l, t, r, b);
  }
};

/// Helper for building Rect's with left/bottom/width/height arguments.
template <class V>
class RectLBWH : public Rect<V>
{
public:
  RectLBWH(V l, V b, V w, V h) :
    Rect<V>()
  {
    setRectXYWH(l, b, w, h);
  }
};

} // end namespace Gfx

static const char vcid_rect_h[] = "$Header$";
#endif // !RECT_H_DEFINED
