///////////////////////////////////////////////////////////////////////
// point.h
// Rob Peters
// created: Thu Jan 28 12:54:13 1999
// written: Mon Nov 15 16:10:48 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POINT_H_DEFINED
#define POINT_H_DEFINED

template<class V>
class Point {
public:
  Point(V x=0, V y=0) : x_(x), y_(y) {}

  V& x() { return x_; }
  V& y() { return y_; }

  const V& x() const { return x_; }
  const V& y() const { return y_; }

  Point operator+(const Point<V>& rhs) const
	 { return Point<V>(x_ + rhs.x(), y_ + rhs.y()); }

  Point operator-(const Point<V>& rhs) const
	 { return Point<V>(x_ - rhs.x(), y_ - rhs.y()); }


  Point operator*(const V& factor) const
	 { return Point<V>(x_ * factor, y_ * factor); }

  Point operator/(const V& factor) const 
	 { return Point<V>(x_ / factor, y_ / factor); }


  template <class U>
  Point& operator+=(const Point<U>& rhs)
	 { x_ += V(rhs.x()); y_ += V(rhs.y()); return *this; }

  template <class U>
  Point& operator-=(const Point<U>& rhs)
	 { x_ -= V(rhs.x()); y_ -= V(rhs.y()); return *this; }


  template <class U>
  Point& operator*=(const U& factor)
	 { x_ *= factor; y_ *= factor; }

  template <class U>
  Point& operator/=(const U& factor)
	 { x_ /= factor; y_ /= factor; }

private:
  V x_;
  V y_;
};

static const char vcid_point_h[] = "$Header$";
#endif // !POINT_H_DEFINED
