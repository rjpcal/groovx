///////////////////////////////////////////////////////////////////////
// point.h
// Rob Peters
// created: Thu Jan 28 12:54:13 1999
// written: Tue Mar 16 19:36:28 1999
// $Id$
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

static const char vcid_point_h[] = "$Header$";
#endif // !POINT_H_DEFINED
