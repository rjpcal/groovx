///////////////////////////////////////////////////////////////////////
//
// txform.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 21 14:00:54 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TXFORM_CC_DEFINED
#define TXFORM_CC_DEFINED

#include "txform.h"

#include "geom/rect.h"
#include "geom/vec3.h"

#include "util/rand.h"

#include <cmath>
#include <cstring>

#define NO_PROF
#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using geom::vec2d;
using geom::vec3d;

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

geom::txform::txform(const vec3d& translation,
                     const vec3d& scaling,
                     const vec3d& rotation_axis,
                     double rotation_angle)
{
DOTRACE("geom::txform::txform(tx, scl, rot)");

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

  const double rl = rotation_axis.length();

  const double rx = rotation_axis.x() / rl;
  const double ry = rotation_axis.y() / rl;
  const double rz = rotation_axis.z() / rl;

  const double c = cos((rotation_angle) * M_PI / 180.0);
  const double s = sin((rotation_angle) * M_PI / 180.0);

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

  m_mtx[0]=sx*(rx*rx*(1-c)+c);    m_mtx[4]=sx*(rx*ry*(1-c)-rz*s); m_mtx[8]=sx*(rx*rz*(1-c)+ry*s); m_mtx[12]=tx;
  m_mtx[1]=sy*(ry*rx*(1-c)+rz*s); m_mtx[5]=sy*(ry*ry*(1-c)+c);    m_mtx[9]=sy*(ry*rz*(1-c)-rx*s); m_mtx[13]=ty;
  m_mtx[2]=sz*(rz*rx*(1-c)-ry*s); m_mtx[6]=sz*(rz*ry*(1-c)+rx*s); m_mtx[10]=sz*(rz*rz*(1-c)+c);   m_mtx[14]=tz;
  m_mtx[3]=0.0;                   m_mtx[7]=0.0;                   m_mtx[11]=0.0;                  m_mtx[15]=1.0;
}

geom::txform geom::txform::identity()
{
DOTRACE("geom::txform::identity");
  txform r(true);
  r[0] = 1.0; r[4] = 0.0; r[8] = 0.0; r[12] = 0.0;
  r[1] = 0.0; r[5] = 1.0; r[9] = 0.0; r[13] = 0.0;
  r[2] = 0.0; r[6] = 0.0; r[10] = 1.0; r[14] = 0.0;
  r[3] = 0.0; r[7] = 0.0; r[11] = 0.0; r[15] = 1.0;
  return r;
}

geom::txform geom::txform::random()
{
DOTRACE("geom::txform::random");

  static rutz::urand generator(rutz::default_rand_seed);

  txform result(true);
  for (int i = 0; i < 16; ++i)
    result.m_mtx[i] = generator.fdraw_range(0.0, 1.0);
  return result;
}

geom::txform geom::txform::orthographic(const geom::rect<double>& b,
                                        double zNear, double zFar)
{
DOTRACE("geom::txform::orthographic");

  txform result = txform::no_init();

  result[0] = 2.0/(b.width());
  result[1] = 0;
  result[2] = 0;
  result[3] = 0;

  result[4] = 0;
  result[5] = 2.0/(b.height());
  result[6] = 0;
  result[7] = 0;

  result[8] = 0;
  result[9] = 0;
  result[10] = -2.0/(zFar - zNear);
  result[11] = 0;

  result[12] = -(b.right()+b.left())/(b.right()-b.left());
  result[13] = -(b.top()+b.bottom())/(b.top()-b.bottom());
  result[14] = -(zFar+zNear)/(zFar-zNear);
  result[15] = 1.0;

  return result;
}

geom::txform geom::txform::inverted() const
{
DOTRACE("geom::txform::inverted");

  // ALGORITHM (Cramer's Rule):

  // 1. Compute the transpose, M' = transpose(M)
  // 2. Compute the cofactor matrix, cof(M')
  // 3. Compute the determinant of the cofactor matrix, det(cof(M'))
  // 4. Compute the inverse, inv(M) = cof(M')/det(cof(M'))

  /*
     |  0  4  8  C  |
     |  1  5  9  D  |
     |  2  6  A  E  |
     |  3  7  B  F  |
  */

#define A 10
#define B 11
#define C 12
#define D 13
#define E 14
#define F 15

  //
  // 1. Compute the transpose, M' = transpose(M)
  //

#define s0 (m_mtx[0])
#define s1 (m_mtx[4])
#define s2 (m_mtx[8])
#define s3 (m_mtx[C])
#define s4 (m_mtx[1])
#define s5 (m_mtx[5])
#define s6 (m_mtx[9])
#define s7 (m_mtx[D])
#define s8 (m_mtx[2])
#define s9 (m_mtx[6])
#define sA (m_mtx[A])
#define sB (m_mtx[E])
#define sC (m_mtx[3])
#define sD (m_mtx[7])
#define sE (m_mtx[B])
#define sF (m_mtx[F])

  //
  // 2. Compute the cofactor matrix, cof(M')
  //

  double cof[16]; // cofactor matrix

  /*
     |  0  4  8  C  |
     |  1  5  9  D  |
     |  2  6  A  E  |
     |  3  7  B  F  |
  */

  {
    // We compute each of the 2x2 determinants in the following form:
    // (upperleft*lowerright - lowerleft*upperright)

    // This makes it easier to verify that we have all the correct +/-
    // signs in the 3x3 determinants (i.e. cofactors) below.

    const double t8D_9C = (s8 * sD) - (s9 * sC);
    const double t8E_AC = (s8 * sE) - (sA * sC);
    const double t8F_BC = (s8 * sF) - (sB * sC);
    const double t9E_AD = (s9 * sE) - (sA * sD);
    const double t9F_BD = (s9 * sF) - (sB * sD);
    const double tAF_BE = (sA * sF) - (sB * sE);

    cof[0]=    + tAF_BE*s5 - t9F_BD*s6 + t9E_AD*s7;
    cof[1]=    - tAF_BE*s4 + t8F_BC*s6 - t8E_AC*s7;
    cof[2]=    + t9F_BD*s4 - t8F_BC*s5 + t8D_9C*s7;
    cof[3]=    - t9E_AD*s4 + t8E_AC*s5 - t8D_9C*s6;

    cof[4]=    - tAF_BE*s1 + t9F_BD*s2 - t9E_AD*s3;
    cof[5]=    + tAF_BE*s0 - t8F_BC*s2 + t8E_AC*s3;
    cof[6]=    - t9F_BD*s0 + t8F_BC*s1 - t8D_9C*s3;
    cof[7]=    + t9E_AD*s0 - t8E_AC*s1 + t8D_9C*s2;
  }

  /*
     |  0  4  8  C  |
     |  1  5  9  D  |
     |  2  6  A  E  |
     |  3  7  B  F  |
  */

  {
    const double t05_14 = (s0 * s5) - (s1 * s4);
    const double t06_24 = (s0 * s6) - (s2 * s4);
    const double t07_34 = (s0 * s7) - (s3 * s4);
    const double t16_25 = (s1 * s6) - (s2 * s5);
    const double t17_35 = (s1 * s7) - (s3 * s5);
    const double t27_36 = (s2 * s7) - (s3 * s6);

    cof[8]=    + t27_36*sD - t17_35*sE + t16_25*sF;
    cof[9]=    - t27_36*sC + t07_34*sE - t06_24*sF;
    cof[A]=    + t17_35*sC - t07_34*sD + t05_14*sF;
    cof[B]=    - t16_25*sC + t06_24*sD - t05_14*sE;

    cof[C]=    - t27_36*s9 + t17_35*sA - t16_25*sB;
    cof[D]=    + t27_36*s8 - t07_34*sA + t06_24*sB;
    cof[E]=    - t17_35*s8 + t07_34*s9 - t05_14*sB;
    cof[F]=    + t16_25*s8 - t06_24*s9 + t05_14*sA;
  }

  //
  // 3. Compute the determinant of the cofactor matrix, det(cof(M'))
  //

  const double det_reciprocal =
    1.0 / (s0*cof[0] +
           s1*cof[1] +
           s2*cof[2] +
           s3*cof[3]);

#undef A
#undef B
#undef C
#undef D
#undef E
#undef F

#undef s0
#undef s1
#undef s2
#undef s3
#undef s4
#undef s5
#undef s6
#undef s7
#undef s8
#undef s9
#undef sA
#undef sB
#undef sC
#undef sD
#undef sE
#undef sF

  //
  // 4. Compute the inverse, inv(M) = cof(M')/det(cof(M'))
  //

  geom::txform inverse(true);

  for (int i = 0; i < 16; ++i)
    {
      inverse.m_mtx[i] = cof[i] * det_reciprocal;
    }

  return inverse;
}

void geom::txform::translate(const vec3d& t)
{
DOTRACE("geom::txform::translate");

#if 0
  /*                     brute force

              | m0 m4 m8  m12 |   |  1  0  0 tx |
     result = | m1 m5 m9  m13 | * |  0  1  0 ty |
              | m2 m6 m10 m14 |   |  0  0  1 tz |
              | m3 m7 m11 m15 |   |  0  0  0  1 |
  */
  txform old_mtx(*this);

  double tm[16];
  tm[0] = 1.0; tm[4] = 0.0;  tm[8] = 0.0; tm[12] = t.x();
  tm[1] = 0.0; tm[5] = 1.0;  tm[9] = 0.0; tm[13] = t.y();
  tm[2] = 0.0; tm[6] = 0.0; tm[10] = 1.0; tm[14] = t.z();
  tm[3] = 0.0; tm[7] = 0.0; tm[11] = 0.0; tm[15] = 1.0;

  mul_mtx_4x4(old_mtx.m_mtx, tm, this->m_mtx);
#else
  /*                 optimized

              | m0 m4 m8  m0*tx+m4*ty+m8*tz+m12  |
     result = | m1 m5 m9  m1*tx+m5*ty+m9*tz+m13  |
              | m2 m6 m10 m2*tx+m6*ty+m10*tz+m14 |
              | m3 m7 m11 m3*tx+m7*ty+m11*tz+m15 |
  */
  m_mtx[12] += t.x()*m_mtx[0] + t.y()*m_mtx[4] + t.z()*m_mtx[8];
  m_mtx[13] += t.x()*m_mtx[1] + t.y()*m_mtx[5] + t.z()*m_mtx[9];
  m_mtx[14] += t.x()*m_mtx[2] + t.y()*m_mtx[6] + t.z()*m_mtx[10];
  m_mtx[15] += t.x()*m_mtx[3] + t.y()*m_mtx[7] + t.z()*m_mtx[11];
#endif
}

void geom::txform::scale(const vec3d& s)
{
DOTRACE("geom::txform::scale");

#if 0
  /*                brute force

              | m0 m4 m8  m12 |   | sx  0  0  0 |
     result = | m1 m5 m9  m13 | * |  0 sy  0  0 |
              | m2 m6 m10 m14 |   |  0  0 sz  0 |
              | m3 m7 m11 m15 |   |  0  0  0  1 |
  */
  txform old_mtx(*this);

  double sm[16];
  sm[0] = s.x(); sm[4] = 0.0;    sm[8] = 0.0;    sm[12] = 0.0;
  sm[1] = 0.0;   sm[5] = s.y();  sm[9] = 0.0;    sm[13] = 0.0;
  sm[2] = 0.0;   sm[6] = 0.0;    sm[10] = s.z(); sm[14] = 0.0;
  sm[3] = 0.0;   sm[7] = 0.0;    sm[11] = 0.0;   sm[15] = 1.0;

  mul_mtx_4x4(old_mtx.m_mtx, sm, this->m_mtx);
#else
  /*                 optimized

              | sx*m0 sy*m4 sz*m8  m12 |
     result = | sx*m1 sy*m5 sz*m9  m13 |
              | sx*m2 sy*m6 sz*m10 m14 |
              | sx*m3 sy*m7 sz*m11 m15 |
  */
  m_mtx[0] *= s.x();
  m_mtx[1] *= s.x();
  m_mtx[2] *= s.x();
  m_mtx[3] *= s.x();
  m_mtx[4] *= s.y();
  m_mtx[5] *= s.y();
  m_mtx[6] *= s.y();
  m_mtx[7] *= s.y();
  m_mtx[8] *= s.z();
  m_mtx[9] *= s.z();
  m_mtx[10] *= s.z();
  m_mtx[11] *= s.z();
#endif
}

void geom::txform::rotate(const vec3d& rotation_axis,
                          double rotation_angle)
{
DOTRACE("geom::txform::rotate");

  txform rotation(vec3d(0, 0, 0),
                  vec3d(1, 1, 1),
                  rotation_axis, rotation_angle);

  this->transform(rotation);
}

geom::txform geom::txform::mtx_mul(const txform& other) const
{
DOTRACE("geom::txform::mtx_mul");

  txform result(true);

  mul_mtx_4x4(this->m_mtx, other.m_mtx, result.m_mtx);

  return result;
}

void geom::txform::transform(const geom::txform& other)
{
DOTRACE("geom::txform::transform");

  txform old_mtx(*this);

  mul_mtx_4x4(old_mtx.m_mtx, other.m_mtx, this->m_mtx);
}

vec2d geom::txform::apply_to(const vec2d& input) const
{
DOTRACE("geom::txform::apply_to(vec2d)");
  /*
        | m0 m4 m8  m12 |   | input.x |
        | m1 m5 m9  m13 | * | input.y |
        | m2 m6 m10 m14 |   |    0    |
        | m3 m7 m11 m15 |   |    1    |
   */

  const double output_x =
    m_mtx[0] * input.x()
    + m_mtx[4] * input.y()
    // + m_mtx[8] * 0.0
    + m_mtx[12] * 1.0;

  const double output_y =
    m_mtx[1] * input.x()
    + m_mtx[5] * input.y()
    // + m_mtx[9] * 0.0
    + m_mtx[13] * 1.0;

  // output_z is forced to zero since we're returning a 2-D result

  const double output_w =
    m_mtx[3] * input.x()
    + m_mtx[7] * input.y()
    // + m_mtx[11] * 0.0
    + m_mtx[15] * 1.0;

  return (output_w != 0.0)
    ? vec2d(output_x / output_w, output_y / output_w)
    : vec2d(0.0, 0.0);
}

vec3d geom::txform::apply_to(const vec3d& input) const
{
DOTRACE("geom::txform::apply_to(vec3d)");
  /*
        | m0 m4 m8  m12 |   | input.x |
        | m1 m5 m9  m13 | * | input.y |
        | m2 m6 m10 m14 |   | input.z |
        | m3 m7 m11 m15 |   |    1    |
   */

  const double output_x =
    m_mtx[0] * input.x()
    + m_mtx[4] * input.y()
    + m_mtx[8] * input.z()
    + m_mtx[12] * 1.0;

  const double output_y =
    m_mtx[1] * input.x()
    + m_mtx[5] * input.y()
    + m_mtx[9] * input.z()
    + m_mtx[13] * 1.0;

  const double output_z =
    m_mtx[2] * input.x()
    + m_mtx[6] * input.y()
    + m_mtx[10] * input.z()
    + m_mtx[14] * 1.0;

  const double output_w =
    m_mtx[3] * input.x()
    + m_mtx[7] * input.y()
    + m_mtx[11] * input.z()
    + m_mtx[15] * 1.0;

  return (output_w != 0.0)
    ? vec3d(output_x / output_w,
            output_y / output_w,
            output_z / output_w)
    : vec3d(0.0, 0.0, 0.0);
}

void geom::txform::set_col_major_data(const double* data)
{
DOTRACE("geom::txform::set_col_major_data");
  for (int i = 0; i < 16; ++i)
    m_mtx[i] = data[i];
}

void geom::txform::debug_dump() const
{
DOTRACE("geom::txform::debug_dump");
  dbg_print(0, m_mtx[0]); dbg_print(0, m_mtx[4]); dbg_print(0, m_mtx[8]); dbg_print_nl(0, m_mtx[12]);
  dbg_print(0, m_mtx[1]); dbg_print(0, m_mtx[5]); dbg_print(0, m_mtx[9]); dbg_print_nl(0, m_mtx[13]);
  dbg_print(0, m_mtx[2]); dbg_print(0, m_mtx[6]); dbg_print(0, m_mtx[10]); dbg_print_nl(0, m_mtx[14]);
  dbg_print(0, m_mtx[3]); dbg_print(0, m_mtx[7]); dbg_print(0, m_mtx[11]); dbg_print_nl(0, m_mtx[15]);
}

double geom::txform::debug_sse(const txform& ref) const
{
DOTRACE("geom::txform::debug_sse");
  double result = 0.0;
  for (int i = 0; i < 16; ++i)
    {
      const double diff = this->m_mtx[i] - ref.m_mtx[i];
      result += (diff*diff);
    }
  return result;
}

static const char vcid_txform_cc[] = "$Id$ $URL$";
#endif // !TXFORM_CC_DEFINED
