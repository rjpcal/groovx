///////////////////////////////////////////////////////////////////////
//
// rect.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Fri Aug 10 11:31:07 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RECT_H_DEFINED
#define RECT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINT_H_DEFINED)
#include "point.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ALGO_H_DEFINED)
#include "util/algo.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// Rect template class definition
//
///////////////////////////////////////////////////////////////////////

template<class V>
class Rect {
public:
  // Creators
  Rect() : ll(), tt(), rr(), bb() {} // everything inits to zero

  Rect(V L, V T, V R, V B) : ll(L), tt(T), rr(R), bb(B) {}

  Rect(const Point<V>& p1, const Point<V>& p2)
    { setCorners(p1, p2); }

  Rect(const Rect<V>& i) : ll(i.ll), tt(i.tt), rr(i.rr), bb(i.bb) {}

  Rect<V>& operator=(const Rect<V>& i)
    { ll=i.ll; tt=i.tt; rr=i.rr; bb=i.bb; return *this; }

  // Accessors
  void getRectLTRB(V& L, V& T, V& R, V& B) const
    { L = ll; T = tt; R = rr; B = bb; }
  void getRectLRBT(V& L, V& R, V& B, V& T) const
    { L = ll; R = rr; B = bb; T = tt; }

  Point<V> bottomLeft() const
    { return Point<V>(ll, bb); }

  Point<V> bottomRight() const
    { return Point<V>(rr, bb); }

  Point<V> topLeft() const
    { return Point<V>(ll, tt); }

  Point<V> topRight() const
    { return Point<V>(rr, tt); }

  V width() const { return (rr-ll); }
  V height() const { return (tt-bb); }

  Point<V> extent() const { return Point<V>(width(), height()); }

  V aspect() const { return width()/height(); }

  Point<V> center() const { return Point<V>(centerX(), centerY()); }

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

  void setCorners(const Point<V>& p1, const Point<V>& p2)
  {
    ll = Util::min(p1.x(), p2.x());
    rr = Util::max(p1.x(), p2.x());
    bb = Util::min(p1.y(), p2.y());
    tt = Util::max(p1.y(), p2.y());
  }

  void setBottomLeft(const Point<V>& point)
    { ll = point.x(); bb = point.y(); }

  void setBottomRight(const Point<V>& point)
    { rr = point.x(); bb = point.y(); }

  void setTopLeft(const Point<V>& point)
    { ll = point.x(); tt = point.y(); }

  void setTopRight(const Point<V>& point)
    { rr = point.x(); tt = point.y(); }

  void widenByFactor(V factor) { ll *= factor; rr *= factor; }
  void heightenByFactor(V factor) { tt *= factor; bb *= factor; }

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
