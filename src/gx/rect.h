///////////////////////////////////////////////////////////////////////
// rect.h
// Rob Peters
// created: Jan-99
// written: Tue Mar 16 19:33:10 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef RECT_H_DEFINED
#define RECT_H_DEFINED

class rect {
public:
  rect() : l(0.0), t(0.0), r(0.0), b(0.0) {}
  rect(const rect& i) : l(i.l), t(i.t), r(i.r), b(i.b) {}
  
  float l, t, r, b;
  void setRectLTRB(float L, float T, float R, float B)
        { l = L; t = T; r = R; b = B; }
  void setRectLRBT(float L, float R, float B, float T)
        { l = L; r = R; b = B; t = T; }
  float width() const { return (r-l); }
  float height() const { return (t-b); }
  float aspect() const { return width()/height(); }
  void widen(float f) { l *= f; r *=f; }
  void heighten(float f) { t *=f; b *=f; }
};

static const char vcid_rect_h[] = "$Header$";
#endif // !RECT_H_DEFINED
