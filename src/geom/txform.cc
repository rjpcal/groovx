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

#include "geom/vec3.h"

#include <cmath>
#include <cstring>

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

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

geom::txform::txform()
{
DOTRACE("geom::txform::txform");
  m_mtx[0] = 1.0; m_mtx[4] = 0.0; m_mtx[8] = 0.0; m_mtx[12] = 0.0;
  m_mtx[1] = 0.0; m_mtx[5] = 1.0; m_mtx[9] = 0.0; m_mtx[13] = 0.0;
  m_mtx[2] = 0.0; m_mtx[6] = 0.0; m_mtx[10] = 1.0; m_mtx[14] = 0.0;
  m_mtx[3] = 0.0; m_mtx[7] = 0.0; m_mtx[11] = 0.0; m_mtx[15] = 1.0;
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

void geom::txform::transform(const geom::txform& other)
{
DOTRACE("geom::txform::transform");

  txform old_mtx(*this);

  mul_mtx_4x4(old_mtx.m_mtx, other.m_mtx, this->m_mtx);
}

vec2d geom::txform::apply_to(const vec2d& input) const
{
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

void geom::txform::debug_dump() const
{
  dbg_print(0, m_mtx[0]); dbg_print(0, m_mtx[4]); dbg_print(0, m_mtx[8]); dbg_print_nl(0, m_mtx[12]);
  dbg_print(0, m_mtx[1]); dbg_print(0, m_mtx[5]); dbg_print(0, m_mtx[9]); dbg_print_nl(0, m_mtx[13]);
  dbg_print(0, m_mtx[2]); dbg_print(0, m_mtx[6]); dbg_print(0, m_mtx[10]); dbg_print_nl(0, m_mtx[14]);
  dbg_print(0, m_mtx[3]); dbg_print(0, m_mtx[7]); dbg_print(0, m_mtx[11]); dbg_print_nl(0, m_mtx[15]);
}

static const char vcid_txform_cc[] = "$Header$";
#endif // !TXFORM_CC_DEFINED
