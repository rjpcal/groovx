///////////////////////////////////////////////////////////////////////
//
// rect.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Mon Aug 13 12:09:16 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RECT_H_DEFINED
#define RECT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VEC2_H_DEFINED)
#include "gfx/vec2.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ALGO_H_DEFINED)
#include "util/algo.h"
#endif

namespace Gfx
{
  template <class V> class Rect;
}

///////////////////////////////////////////////////////////////////////
//
// Rect template class definition
//
///////////////////////////////////////////////////////////////////////

template<class V>
class Gfx::Rect {
public:
  // Creators
  Rect() : ll(), tt(), rr(), bb() {} // everything inits to zero

  Rect(V L, V T, V R, V B) : ll(L), tt(T), rr(R), bb(B) {}

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

  Gfx::Vec2<V> extent() const { return Gfx::Vec2<V>(width(), height()); }

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

  void translate(const Gfx::Vec2<V>& dist)
  {
    ll += dist.x(); rr += dist.x();
    bb += dist.y(); tt += dist.y();
  }

  void scale(const Gfx::Vec2<V>& factors)
  {
    widenByFactor(factors.x());
    heightenByFactor(factors.y());
  }

  void setCenter(const Gfx::Vec2<V>& point)
  {
    Gfx::Vec2<V> diff = point - center();
    translate(diff);
  }

  void widenByFactor(V factor) { ll *= factor; rr *= factor; }
  void heightenByFactor(V factor) { bb *= factor; tt *= factor; }

  void widenByStep(V step) { ll -= step; rr += step; }
  void heightenByStep(V step) { bb -= step; tt += step; }

  void isVoid() const { return (tt == bb) || (ll == rr); }

  void unionize(const Rect<double>& other)
  {
    ll = Util::min(ll, other.ll);
    rr = Util::max(rr, other.rr);
    bb = Util::min(bb, other.bb);
    tt = Util::max(tt, other.tt);
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

static const char vcid_rect_h[] = "$Header$";
#endif // !RECT_H_DEFINED
