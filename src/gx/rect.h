///////////////////////////////////////////////////////////////////////
//
// rect.h
// Rob Peters
// created: Jan-99
// written: Wed Oct 13 09:09:55 1999
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
  Rect() : l(), t(), r(), b() {}	// everything inits to zero

  Rect(V L, V T, V R, V B) : l(L), t(T), r(R), b(B) {}

  Rect(const Rect<V>& i) : l(i.l), t(i.t), r(i.r), b(i.b) {}
  
  V l, t, r, b;
  void setRectLTRB(V L, V T, V R, V B)
        { l = L; t = T; r = R; b = B; }
  void setRectLRBT(V L, V R, V B, V T)
        { l = L; r = R; b = B; t = T; }
  V width() const { return (r-l); }
  V height() const { return (t-b); }
  V aspect() const { return width()/height(); }
  void widen(V f) { l *= f; r *=f; }
  void heighten(V f) { t *=f; b *=f; }
};

static const char vcid_rect_h[] = "$Header$";
#endif // !RECT_H_DEFINED
