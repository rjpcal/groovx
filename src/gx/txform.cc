///////////////////////////////////////////////////////////////////////
//
// txform.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 21 14:00:54 2002
// written: Tue Nov 19 18:12:35 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TXFORM_CC_DEFINED
#define TXFORM_CC_DEFINED

#include "gx/txform.h"

#include "gx/vec3.h"

#include <cmath>
#include <cstring>

#include "util/trace.h"

namespace
{
  void mul_mtx_4x4(const double* m1, const double* m2, double* result)
  {
    // Note we're using column-major order here
    for (int result_col = 0; result_col < 4; ++result_col)
      for (int result_row = 0; result_row < 4; ++result_row)
        {
          result[result_col * 4 + result_row] =
              m1[0 * 4 + result_row] * m2[result_col * 4 + 0]
            + m1[1 * 4 + result_row] * m2[result_col * 4 + 1]
            + m1[2 * 4 + result_row] * m2[result_col * 4 + 2]
            + m1[3 * 4 + result_row] * m2[result_col * 4 + 3];
        }
  }
}

Gfx::Txform::Txform()
{
DOTRACE("Gfx::Txform::Txform");
  data[0] = 1.0; data[4] = 0.0; data[8] = 0.0; data[12] = 0.0;
  data[1] = 0.0; data[5] = 1.0; data[9] = 0.0; data[13] = 0.0;
  data[2] = 0.0; data[6] = 0.0; data[10] = 1.0; data[14] = 0.0;
  data[3] = 0.0; data[7] = 0.0; data[11] = 0.0; data[15] = 1.0;
}


Gfx::Txform::Txform(const Vec3<double>& translation,
                    const Vec3<double>& scaling,
                    const Vec3<double>& rotationAxis,
                    double rotationAngle)
{
DOTRACE("Gfx::Txform::Txform(tx, scl, rot)");

  // The matrices look transposed because OpenGL expects column-major

  const double sx = scaling.x();
  const double sy = scaling.y();
  const double sz = scaling.z();

  const double tx = translation.x();
  const double ty = translation.y();
  const double tz = translation.z();

  // The result of (1) translation, then (2) scaling
  /*
    const double ts[16] =
    {
    sx,   0.0, 0.0, 0.0,
    0.0,  sy,  0.0, 0.0,
    0.0,  0.0, sz,  0.0,
    tx,   ty,  tz,  1.0
    };
  */

  const double rl = rotationAxis.length();

  const double rx = rotationAxis.x() / rl;
  const double ry = rotationAxis.y() / rl;
  const double rz = rotationAxis.z() / rl;

  const double c = cos((rotationAngle) * M_PI / 180.0);
  const double s = sin((rotationAngle) * M_PI / 180.0);

  // The result of rotation
  /*
    const double r[16] =
    {
    rx*rx*(1-c)+c,    ry*rx*(1-c)+rz*s, rx*rz*(1-c)-ry*s, 0.0,
    rx*ry*(1-c)-rz*s, ry*ry*(1-c)+c,    ry*rz*(1-c)+rx*s, 0.0,
    rx*rz*(1-c)+ry*s, ry*rz*(1-c)-rx*s, rz*rz*(1-c)+c,    0.0,
    0.0,              0.0,              0.0,              1.0
    };
  */

  // Compute result = t * s * r = ts * r;
  /* mul_mtx_4x4(ts, r, result); */

  data[0]=sx*(rx*rx*(1-c)+c);    data[4]=sx*(rx*ry*(1-c)-rz*s); data[8]=sx*(rx*rz*(1-c)+ry*s); data[12]=tx;
  data[1]=sy*(ry*rx*(1-c)+rz*s); data[5]=sy*(ry*ry*(1-c)+c);    data[9]=sy*(ry*rz*(1-c)-rx*s); data[13]=ty;
  data[2]=sz*(rz*rx*(1-c)-ry*s); data[6]=sz*(rz*ry*(1-c)+rx*s); data[10]=sz*(rz*rz*(1-c)+c);   data[14]=tz;
  data[3]=0.0;                   data[7]=0.0;                   data[11]=0.0;                  data[15]=1.0;
}

void Gfx::Txform::translate(const Vec3<double>& t)
{
  Txform old_mtx(*this);

  double tm[16];
  tm[0] = 1.0; tm[4] = 0.0;  tm[8] = 0.0; tm[12] = t.x();
  tm[1] = 0.0; tm[5] = 1.0;  tm[9] = 0.0; tm[13] = t.y();
  tm[2] = 0.0; tm[6] = 0.0; tm[10] = 1.0; tm[14] = t.z();
  tm[3] = 0.0; tm[7] = 0.0; tm[11] = 0.0; tm[15] = 1.0;

  mul_mtx_4x4(old_mtx.data, tm, this->data);
}

void Gfx::Txform::scale(const Vec3<double>& s)
{
  Txform old_mtx(*this);

  double sm[16];
  sm[0] = s.x(); sm[4] = 0.0;    sm[8] = 0.0;    sm[12] = 0.0;
  sm[1] = 0.0;   sm[5] = s.y();  sm[9] = 0.0;    sm[13] = 0.0;
  sm[2] = 0.0;   sm[6] = 0.0;    sm[10] = s.z(); sm[14] = 0.0;
  sm[3] = 0.0;   sm[7] = 0.0;    sm[11] = 0.0;   sm[15] = 1.0;

  mul_mtx_4x4(old_mtx.data, sm, this->data);
}

void Gfx::Txform::transform(const Gfx::Txform& other)
{
  Txform old_mtx(*this);

  mul_mtx_4x4(old_mtx.data, other.data, this->data);
}

Gfx::Vec2<double> Gfx::Txform::applyTo(const Gfx::Vec2<double>& input) const
{
  /*
        | m0 m4 m8  m12 |   | input.x |
        | m1 m5 m9  m13 | * | input.y |
        | m2 m6 m10 m14 |   |    0    |
        | m3 m7 m11 m15 |   |    1    |
   */

  const double output_x =
    data[0] * input.x()
    + data[4] * input.y()
    // + data[8] * 0.0
    + data[12] * 1.0;

  const double output_y =
    data[1] * input.x()
    + data[5] * input.y()
    // + data[9] * 0.0
    + data[13] * 1.0;

  // output_z is forced to zero since we're returning a 2-D result

  const double output_w =
    data[3] * input.x()
    + data[7] * input.y()
    // + data[11] * 0.0
    + data[15] * 1.0;

  return (output_w != 0.0)
    ? Gfx::Vec2<double>(output_x / output_w, output_y / output_w)
    : Gfx::Vec2<double>(0.0, 0.0);
}

Gfx::Vec3<double> Gfx::Txform::applyTo(const Gfx::Vec3<double>& input) const
{
  /*
        | m0 m4 m8  m12 |   | input.x |
        | m1 m5 m9  m13 | * | input.y |
        | m2 m6 m10 m14 |   | input.z |
        | m3 m7 m11 m15 |   |    1    |
   */

  const double output_x =
    data[0] * input.x()
    + data[4] * input.y()
    + data[8] * input.z()
    + data[12] * 1.0;

  const double output_y =
    data[1] * input.x()
    + data[5] * input.y()
    + data[9] * input.z()
    + data[13] * 1.0;

  const double output_z =
    data[2] * input.x()
    + data[6] * input.y()
    + data[10] * input.z()
    + data[14] * 1.0;

  const double output_w =
    data[3] * input.x()
    + data[7] * input.y()
    + data[11] * input.z()
    + data[15] * 1.0;

  return (output_w != 0.0)
    ? Gfx::Vec3<double>(output_x / output_w,
                        output_y / output_w,
                        output_z / output_w)
    : Gfx::Vec3<double>(0.0, 0.0, 0.0);
}

static const char vcid_txform_cc[] = "$Header$";
#endif // !TXFORM_CC_DEFINED
