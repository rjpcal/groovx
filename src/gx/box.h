///////////////////////////////////////////////////////////////////////
//
// box.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul  3 15:03:23 2002
// written: Thu Nov 14 19:11:35 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BOX_H_DEFINED
#define BOX_H_DEFINED

#include "gx/vec3.h"
#include "gx/rect.h"

#include "util/algo.h"
#include "util/debug.h"

namespace Gfx
{

template <class V>
class Box
{
private:
  V xx0, xx1, yy0, yy1, zz0, zz1;

public:
  Box() : xx0(), xx1(), yy0(), yy1(), zz0(), zz1() {} // init all to zero

  Box(const Rect<V>& rect) :
    xx0(rect.left()), xx1(rect.right()),
    yy0(rect.bottom()), yy1(rect.top()),
    zz0(), zz1()
  {}

  Rect<V> rect() const
  {
    Rect<V> result;
    result.setRectLRBT(xx0, xx1, yy0, yy1);
    return result;
  }

  V x0() const { return xx0; }
  V x1() const { return xx1; }
  V y0() const { return yy0; }
  V y1() const { return yy1; }
  V z0() const { return zz0; }
  V z1() const { return zz1; }

  Vec3<V> point000() const { return Gfx::Vec3<V>(xx0, yy0, zz0); }
  Vec3<V> point001() const { return Gfx::Vec3<V>(xx0, yy0, zz1); }
  Vec3<V> point010() const { return Gfx::Vec3<V>(xx0, yy1, zz0); }
  Vec3<V> point011() const { return Gfx::Vec3<V>(xx0, yy1, zz1); }
  Vec3<V> point100() const { return Gfx::Vec3<V>(xx1, yy0, zz0); }
  Vec3<V> point101() const { return Gfx::Vec3<V>(xx1, yy0, zz1); }
  Vec3<V> point110() const { return Gfx::Vec3<V>(xx1, yy1, zz0); }
  Vec3<V> point111() const { return Gfx::Vec3<V>(xx1, yy1, zz1); }

  V sizeX() const { return xx1 - xx0; }
  V sizeY() const { return yy1 - yy0; }
  V sizeZ() const { return zz1 - zz0; }

  V centerX() const { return (xx0+xx1)/V(2); }
  V centerY() const { return (yy0+yy1)/V(2); }
  V centerZ() const { return (zz0+zz1)/V(2); }

  Gfx::Vec3<V> center() const
  { return Gfx::Vec3<V>(centerX(), centerY(), centerZ()); }

  void set000(const Vec3<V>& pt) { xx0=pt.x(); yy0=pt.y(); zz0=pt.z(); }
  void set001(const Vec3<V>& pt) { xx0=pt.x(); yy0=pt.y(); zz1=pt.z(); }
  void set010(const Vec3<V>& pt) { xx0=pt.x(); yy1=pt.y(); zz0=pt.z(); }
  void set011(const Vec3<V>& pt) { xx0=pt.x(); yy1=pt.y(); zz1=pt.z(); }
  void set100(const Vec3<V>& pt) { xx1=pt.x(); yy0=pt.y(); zz0=pt.z(); }
  void set101(const Vec3<V>& pt) { xx1=pt.x(); yy0=pt.y(); zz1=pt.z(); }
  void set110(const Vec3<V>& pt) { xx1=pt.x(); yy1=pt.y(); zz0=pt.z(); }
  void set111(const Vec3<V>& pt) { xx1=pt.x(); yy1=pt.y(); zz1=pt.z(); }

  void setXXYYZZ(V x0, V x1, V y0, V y1, V z0, V z1)
  {
    xx0 = x0; xx1 = x1;
    yy0 = y0; yy1 = y1;
    zz0 = z0; zz1 = z1;
  }

  /// Like setXXYYZZ(), except figure out the right min/max ordering.
  void setAnyXXYYZZ(V x0, V x1, V y0, V y1, V z0, V z1)
  {
    xx0 = Util::min(x0, x1); xx1 = Util::max(x0, x1);
    yy0 = Util::min(y0, y1); yy1 = Util::max(y0, y1);
    zz0 = Util::min(z0, z1); zz1 = Util::max(z0, z1);
  }

  void setCorners(const Gfx::Vec3<V>& p1, const Gfx::Vec3<V>& p2)
  {
    setAnyXXYYZZ(p1.x(), p2.x(), p1.y(), p2.y(), p1.z(), p2.z());
  }

  void sizeX(V s) { V del = s - sizeX(); xx0-=0.5*del; xx1+=0.5*del; }
  void sizeY(V s) { V del = s - sizeY(); yy0-=0.5*del; yy1+=0.5*del; }
  void sizeZ(V s) { V del = s - sizeZ(); zz0-=0.5*del; zz1+=0.5*del; }

  void scaleX(V factor) { sizeX(sizeX() * factor); }
  void scaleY(V factor) { sizeY(sizeY() * factor); }
  void scaleZ(V factor) { sizeZ(sizeZ() * factor); }

  void scale(const Gfx::Vec3<V>& factors)
  {
    scaleX(factors.x());
    scaleY(factors.y());
    scaleZ(factors.z());
  }

  void scale(V factor)
  {
    scaleX(factor);
    scaleY(factor);
    scaleZ(factor);
  }

  void translate(const Gfx::Vec3<V>& dist)
  {
    xx0 += dist.x(); xx1 += dist.x();
    yy0 += dist.y(); yy1 += dist.y();
    zz0 += dist.z(); zz1 += dist.z();
  }

  void setCenter(const Gfx::Vec3<V>& point)
  {
    Gfx::Vec3<V> diff = point - center();
    translate(diff);
  }

  bool isVoid() const { return (xx0>=xx1) && (yy0>=yy1) && (zz0>=zz1); }

  void unionize(const Box<V>& other)
  {
    if (!other.isVoid())
      {
        if (this->isVoid())
          {
            *this = other;
          }
        else
          {
            xx0 = Util::min(xx0, other.xx0);
            xx1 = Util::max(xx1, other.xx1);
            yy0 = Util::min(yy0, other.yy0);
            yy1 = Util::max(yy1, other.yy1);
            zz0 = Util::min(zz0, other.zz0);
            zz1 = Util::max(zz1, other.zz1);
          }
      }
  }

  void merge(const Gfx::Vec3<V>& p)
  {
    if (p.x() < xx0) xx0 = p.x(); else if (p.x() > xx1) xx1 = p.x();
    if (p.y() < yy0) yy0 = p.y(); else if (p.y() > yy1) yy1 = p.y();
    if (p.z() < zz0) zz0 = p.z(); else if (p.z() > zz1) zz1 = p.z();
  }

  void debugDump() const
  {
    dbgEval(0, xx0); dbgEvalNL(0, xx1);
    dbgEval(0, yy0); dbgEvalNL(0, yy1);
    dbgEval(0, zz0); dbgEvalNL(0, zz1);
  }
};

}

static const char vcid_box_h[] = "$Header$";
#endif // !BOX_H_DEFINED
