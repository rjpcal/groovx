///////////////////////////////////////////////////////////////////////
// point.h
// Rob Peters
// created: Thu Jan 28 12:54:13 1999
// written: Mon Nov 15 14:33:09 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POINT_H_DEFINED
#define POINT_H_DEFINED

template<class V>
class Point {
public:
  Point(V x=0, V y=0) : x_(x), y_(y);
  virtual ~Point();

  V& x() { return x_; }
  V& y() { return y_; }

  const V& x() const { return x_; }
  const V& y() const { return y_; }

private:
  V x_;
  V y_;
};

static const char vcid_point_h[] = "$Header$";
#endif // !POINT_H_DEFINED
