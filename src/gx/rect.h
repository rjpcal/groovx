///////////////////////////////////////////////////////////////////////
//
// rect.h
// Rob Peters
// created: Jan-99
// written: Mon Nov 15 14:07:31 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RECT_H_DEFINED
#define RECT_H_DEFINED

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

  Rect(const Rect<V>& i) : l(i.l), t(i.t), r(i.r), b(i.b) {}
  
  Rect<V>& operator=(const Rect<V>& i)
	 { l=i.l; t=i.t; r=i.r; b=i.b; return *this; }

  // Accessors
  void getRectLTRB(V& L, V& T, V& R, V& B) const
        { L = l; T = t; R = r; B = b; }
  void getRectLRBT(V& L, V& R, V& B, V& T) const
        { L = l; R = r; B = b; T = t; }

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
