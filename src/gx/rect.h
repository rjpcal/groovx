///////////////////////////////////////////////////////////////////////
//
// rect.h
// Rob Peters
// created: Jan-99
// written: Wed Mar 29 14:07:35 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RECT_H_DEFINED
#define RECT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINT_H_DEFINED)
#include "point.h"
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
  Rect() : l(), t(), r(), b() {}	// everything inits to zero

  Rect(V L, V T, V R, V B) : l(L), t(T), r(R), b(B) {}

  Rect(const Point<V>& p1, const Point<V>& p2)
	 { setCorners(p1, p2); }

  Rect(const Rect<V>& i) : l(i.l), t(i.t), r(i.r), b(i.b) {}
  
  Rect<V>& operator=(const Rect<V>& i)
	 { l=i.l; t=i.t; r=i.r; b=i.b; return *this; }

  // Accessors
  void getRectLTRB(V& L, V& T, V& R, V& B) const
	 { L = l; T = t; R = r; B = b; }
  void getRectLRBT(V& L, V& R, V& B, V& T) const
	 { L = l; R = r; B = b; T = t; }

  Point<V> bottomLeft() const
	 { return Point<V>(l, b); }

  Point<V> bottomRight() const
	 { return Point<V>(r, b); }

  Point<V> topLeft() const
	 { return Point<V>(l, t); }

  Point<V> topRight() const
	 { return Point<V>(r, t); }

  V width() const { return (r-l); }
  V height() const { return (t-b); }
  V aspect() const { return width()/height(); }

  V centerX() const { return (r+l)/V(2); }
  V centerY() const { return (t+b)/V(2); }

  // Manipulators
  void setRectLTRB(V L, V T, V R, V B)
	 { l = L; t = T; r = R; b = B; }
  void setRectLRBT(V L, V R, V B, V T)
	 { l = L; r = R; b = B; t = T; }

  void setCorners(const Point<V>& p1, const Point<V>& p2)
	 {
		l = p1.x() < p2.x() ? p1.x() : p2.x();
		r = p1.x() > p2.x() ? p1.x() : p2.x();
		b = p1.y() < p2.y() ? p1.y() : p2.y();
		t = p1.y() > p2.y() ? p1.y() : p2.y();
	 }

  void setBottomLeft(const Point<V>& point)
	 { l = point.x(); b = point.y(); }

  void setBottomRight(const Point<V>& point)
	 { r = point.x(); b = point.y(); }

  void setTopLeft(const Point<V>& point)
	 { l = point.x(); t = point.y(); }

  void setTopRight(const Point<V>& point)
	 { r = point.x(); t = point.y(); }

  void widenByFactor(V factor) { l *= factor; r *= factor; }
  void heightenByFactor(V factor) { t *= factor; b *= factor; }

  void widenByStep(V step) { l -= step; r += step; }
  void heightenByStep(V step) { b -= step; t += step; }

  V& left() { return l; }
  V& right() { return r; }
  V& bottom() { return b; }
  V& top() { return t; }

  const V& left() const { return l; }
  const V& right() const { return r; }
  const V& bottom() const { return b; }
  const V& top() const { return t; }

private:
  // Data members
  V l, t, r, b;
};

static const char vcid_rect_h[] = "$Header$";
#endif // !RECT_H_DEFINED
