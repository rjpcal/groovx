///////////////////////////////////////////////////////////////////////
// point.h
// Rob Peters
// created: Thu Jan 28 12:54:13 1999
// written: Fri Mar 12 12:55:30 1999
static const char vcid_point_h[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef POINT_H_DEFINED
#define POINT_H_DEFINED

template<class F>
class Point {
public:
  Point(F xi=0, F yi=0) : x(xi), y(yi);
  virtual ~Point();
  F x;
  F y;
};

#endif // !POINT_H_DEFINED
