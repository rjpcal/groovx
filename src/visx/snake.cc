///////////////////////////////////////////////////////////////////////
//
// snake.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 12 11:15:20 2003
// written: Tue May 13 11:28:38 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef SNAKE_CC_DEFINED
#define SNAKE_CC_DEFINED

#include "snake.h"

#include "gx/geom.h"
#include "gx/vec2.h"

#include <algorithm>
#include <cmath>

#include "util/debug.h"
#include "util/trace.h"

using namespace Geom;
using namespace Gfx;

namespace
{
  const double MAX_DELTA         = M_PI/4.0; // == 45 degrees
  const double TEMPERATURE       = 0.05;
  const double INITIAL_INCREMENT = 0.1;

  struct Tuple4
  {
    Tuple4() {}

    Tuple4(double a0, double a1, double a2, double a3)
    { arr[0] = a0; arr[1] = a1; arr[2] = a2; arr[3] = a3; }

    double  operator[](int i) const { return arr[i]; }
    double& operator[](int i)       { return arr[i]; }

    double arr[4];
  };

  void pickRandom4(int length, int i[])
  {
    i[0] = i[1] = i[2] = i[3] = int(length * drand48());

    while (i[1] == i[0])
    {
      i[1] = int(length * drand48());
    }

    while (i[2] == i[0] || i[2] == i[1])
    {
      i[2] = int(length * drand48());
    }

    while (i[3] == i[0] || i[3] == i[1] || i[3] == i[2])
    {
      i[3] = int(length * drand48());
    }

    std::sort(i, i+4);
  }

  Tuple4 getEdgeLengths(const Vec2d no[4])
  {
    return Tuple4(no[0].distanceTo(no[1]),
                  no[1].distanceTo(no[2]),
                  no[2].distanceTo(no[3]),
                  no[3].distanceTo(no[0]));
  }

  Tuple4 getThetas(const Vec2d no[4])
  {
    return Tuple4(no[0].angleTo(no[1]),
                  no[1].angleTo(no[2]),
                  no[2].angleTo(no[3]),
                  no[3].angleTo(no[0]));
  }

  Tuple4 getAlphas(const Tuple4& theta)
  {
    return Tuple4(rad_0_2pi(M_PI - theta[0] + theta[3]),
                  rad_0_2pi(M_PI - theta[1] + theta[0]),
                  rad_0_2pi(M_PI - theta[2] + theta[1]),
                  rad_0_2pi(M_PI - theta[3] + theta[2]));
  }

  // Must return "true" in order to proceed with new nodes in jiggle().
  bool squashQuadrangle(const Vec2d& old_0, const Vec2d& old_1,
                        const Vec2d& old_2, const Vec2d& old_3,
                        Vec2d* new_0, Vec2d* new_1,
                        Vec2d* new_2, Vec2d* new_3,
                        double new_theta)
  {
  DOTRACE("squashQuadrangle");
    {
      // Here we adjust the position of node-1 by jiggling its angle with
      // respect to node-0:

      const double d = old_0.distanceTo(old_1);

      new_1->x() = old_0.x() + d * cos(new_theta);
      new_1->y() = old_0.y() + d * sin(new_theta);
    }

    /*                                                     */
    /*                             . (2)                   */
    /*                           ./|                       */
    /*                  b      ./  `.                      */
    /*                       ./     |                      */
    /*                     ./       `.                     */
    /*                   ./          |                     */
    /*                 ./            `.  c                 */
    /*       (NEW 1) ./_________      |                    */
    /*                `--.  ang       `.                   */
    /*                    `--.         |                   */
    /*                        `--.     `.                  */
    /*                      a     `--.  |                  */
    /*                                `--. (3)             */
    /*                                                     */


    /*

    OK, we've picked a new node-1... now we want to hold node-3 fixed and
    try to find a node-2 that will keep the other distances constant. In
    trigonometric terms, we're trying to solve for the missing vertex of a
    triangle, given (1) the positions of the other two vertices, and (2)
    the lengths of all three sides.

    We do this in two steps:

      (1) assume node-1 is at (0,0), the origin
             and node-3 is at (a,0)

          then find node-2 in this unrotated coord system

      (2) now take the rotation angle (ang) required to put node-3 back in
          its proper position, and rotate node-2 by that angle as well

    From http://mathworld.wolfram.com/Triangle.html:

    A triangle with sides a, b, and c can be constructed by selecting
    vertices (0,0), (a,0), and (x,y), then solving:

        x^2   + y^2  =  b^2
      (x-a)^2 + y^2  =  c^2

    to obtain:

           a^2 + b^2 - c^2
       x = ---------------
                 2a

                    ,------------------------------
                   /          (a^2 + b^2 - c^2)^2
       y = +/- _  /  b^2  -  ---------------------
                \/                   4a^2

                   ,-----------------------------------------------
                -\/  2(a^2b^2 + b^2c^2 + a^2c^2) - a^4 - b^4 - c^4
         = +/- -----------------------------------------------------
                                       2a

    (note that the website has an incorrect formula for the solved y!)

    */

    const double a = old_3.distanceTo(*new_1);
    const double b = old_1.distanceTo(old_2);
    const double c = old_2.distanceTo(old_3);

    const double cos_ang = (old_3.x() - new_1->x()) / a;  // adjac / hypot
    const double sin_ang = (old_3.y() - new_1->y()) / a;  // oppos / hypot

    const double a_sq = a*a;
    const double b_sq = b*b;
    const double c_sq = c*c;

    const double det =
      2*(a_sq*b_sq + b_sq*c_sq + a_sq*c_sq)
      - a_sq*a_sq - b_sq*b_sq - c_sq*c_sq;

    if (det < 0)
      return false;

    // OK, here's the unrotated coords of node-2:

    const double xp = (a*a + b*b - c*c) / (2.0*a);
    const double yp = sqrt(det) / (2.0*a);

    // Here's the two candidate re-rotated coords:

    const Vec2d new_2a
      (new_1->x() + xp*cos_ang - yp*sin_ang,
       new_1->y() + xp*sin_ang + yp*cos_ang);

    const Vec2d new_2b
      (new_1->x() + xp*cos_ang + yp*sin_ang,
       new_1->y() + xp*sin_ang - yp*cos_ang);

    const double d_2_2a = new_2a.distanceTo(old_2);
    const double d_2_2b = new_2b.distanceTo(old_2);

    *new_0 = old_0;
    *new_2 = (d_2_2a < d_2_2b) ? new_2a : new_2b;
    *new_3 = old_3;

    return true;
  }

  bool deltaTooBig(const Tuple4& delta)
  {
    return (fabs(delta[0]) > MAX_DELTA
            || fabs(delta[1]) > MAX_DELTA
            || fabs(delta[2]) > MAX_DELTA
            || fabs(delta[3]) > MAX_DELTA);
  }

  bool acceptNewDelta(const Tuple4& newdelta, const Tuple4& olddelta)
  {
    const double energy_diff =
      newdelta[0]*newdelta[0] - olddelta[0]*olddelta[0]
      + newdelta[1]*newdelta[1] - olddelta[1]*olddelta[1]
      + newdelta[2]*newdelta[2] - olddelta[2]*olddelta[2]
      + newdelta[3]*newdelta[3] - olddelta[3]*olddelta[3];

    // Note, if energy_diff is < 0, then the probability is > 0, so
    // as expected drand48() will always be < probability.
    const double probability = exp(-energy_diff/TEMPERATURE);

    return drand48() <= probability;
  }
}

Snake::Snake(int l, double sp) :
  itsLength(l),
  itsSpacing(sp),
  itsElem(itsLength)
{
DOTRACE("Snake::Snake");

  const double radius = (itsLength * itsSpacing) / (2*M_PI);

  const double alpha_start = 2 * M_PI * drand48();

  for (int n = 0; n < itsLength; ++n)
    {
      const double alpha = alpha_start + 2 * M_PI * n / itsLength;

      itsElem[n].setPolarRad(radius, -alpha);
    }

  const int ITERS = 400;

  for (int count = 0; count < ITERS; ++count)
    {
      this->jiggle();
    }

  // Now reset so that the center of the ring is at the origin
  Vec2d c;

  for (int n = 0; n < itsLength; ++n)
    c += itsElem[n];

  c /= itsLength;

  for (int n = 0; n < itsLength; ++n)
    itsElem[n] -= c;
}

Snake::~Snake()
{
DOTRACE("Snake::~Snake");
}

Element Snake::getElement(int n) const
{
DOTRACE("Snake::getElement");

  Assert(n < itsLength);

  Element result;

  result.type = Element::CONTOUR;
  result.pos.x() = 0.5 * (elem(n).x() + elem(n+1).x());
  result.pos.y() = 0.5 * (elem(n).y() + elem(n+1).y());
  result.theta = rad_0_2pi(-elem(n).angleTo(elem(n+1)));

  return result;
}

/*        (pos.x[n],ypos[n])     position n                               */
/*        (xpos[n+1],ypos[n+1]) position n+1                              */
/*        (0.5*(xpos[n]+xpos[n+1]), 0.5*(ypos[n]+ypos[n+1]))              */
/*                              location of patch n                       */
/*        theta                 angle between pos n and pos n+1           */
/*                              and orientation of patch n                */
/*        delta                 angle difference between patch n and n+1  */
/*                                                                        */
/*                      theta[0]                                          */
/*                             no[0]__________                            */
/*                           ,-~ `-.                                      */
/*                        ,-~       `-.                                   */
/*                     ,-~   alpha[0]  `-.                                */
/*             a[0] ,-~                   `-.  a[3]                       */
/*               ,-~                         `-.                          */
/*            ,-~                               `-.                       */
/*         ,-~                                     `-.                    */
/*      ,-~                                           `-.                 */
/*no[1]~                                                 `-.   theta[3]   */
/*    \   alpha[1]                                          `-.           */
/*     \                                          alpha[3]    no[3]------ */
/*      \                                                 _,-~            */
/*       \                                            _,-~                */
/*        \                                       _,-~                    */
/*         \                                  _,-~                        */
/*      a[1]\                             _,-~                            */
/*           \                        _,-~                                */
/*            \                   _,-~   a[2]                             */
/*             \  alpha[2]    _,-~                                        */
/*              \         _,-~                                            */
/*               \    _,-~   theta[2]                                     */
/*              no[2]~_________                                           */

void Snake::jiggle()
{
DOTRACE("Snake::jiggle");

  int i[4];
  pickRandom4(itsLength, i);

  const Vec2d old_pos[4] =
    {
      itsElem[i[0]], itsElem[i[1]], itsElem[i[2]], itsElem[i[3]]
    };

  const Tuple4 old_theta = getThetas(old_pos);
  const Tuple4 old_alpha = getAlphas(old_theta);

  const Tuple4 old_delta
    (rad_npi_pi(elem(i[0]).angleTo(elem(i[0]+1)) - elem(i[0]-1).angleTo(elem(i[0]))),
     rad_npi_pi(elem(i[1]).angleTo(elem(i[1]+1)) - elem(i[1]-1).angleTo(elem(i[1]))),
     rad_npi_pi(elem(i[2]).angleTo(elem(i[2]+1)) - elem(i[2]-1).angleTo(elem(i[2]))),
     rad_npi_pi(elem(i[3]).angleTo(elem(i[3]+1)) - elem(i[3]-1).angleTo(elem(i[3]))));

  Vec2d new_pos[4];

  double increment = INITIAL_INCREMENT;

  for (;;)
    {
      const double incr = (drand48()<0.5) ? -increment : increment;

      const int r = int(4 * drand48());

      const double incr_theta = old_theta[r] - incr;

      const bool ok =
        squashQuadrangle(old_pos[(r+0)%4], old_pos[(r+1)%4],
                         old_pos[(r+2)%4], old_pos[(r+3)%4],
                         &new_pos[(r+0)%4], &new_pos[(r+1)%4],
                         &new_pos[(r+2)%4], &new_pos[(r+3)%4],
                         incr_theta);

      if (!ok)
        {
          increment *= 0.5;
          continue;
        }

      const Tuple4 new_theta = getThetas(new_pos);
      const Tuple4 new_alpha = getAlphas(new_theta);
      const Tuple4 new_delta
        (old_delta[0] + (old_alpha[0]-new_alpha[0]),
         old_delta[1] + (old_alpha[1]-new_alpha[1]),
         old_delta[2] + (old_alpha[2]-new_alpha[2]),
         old_delta[3] + (old_alpha[3]-new_alpha[3]));

      if (deltaTooBig(new_delta))
        {
          increment *= 0.8;
          continue;
        }

      if (acceptNewDelta(new_delta, old_delta))
        break;
    }

  for (int n = 0; n < 4; ++n)
    this->transformPath(i[n], new_pos[n],
                        i[(n+1)%4], new_pos[(n+1)%4]);

  for (int n = 0; n < 4; ++n)
    {
      itsElem[i[n]] = new_pos[n];
    }
}

void Snake::transformPath(int i1, const Vec2d& new1,
                          int i2, const Vec2d& new2)
{
DOTRACE("Snake::transformPath");

  const Vec2d old1 = itsElem[i1];
  const Vec2d old2 = itsElem[i2];

  // OK, our jiggling algorithm has determined new locations for node-1 and
  // node-2. Now, we want to adjust all the intermediate points
  // accordingly. This amounts to (1) a translation (move node-1 from its
  // old to new location), (2) a rotation (rotate node-2 around node-1 to
  // its new location), and (3) a scaling (contract or expand to put node-2
  // in the right spot). We can then apply these same transformations to
  // the intermediate points.

  /*                                                                   */
  /*               scale by      rotate by (a'-a)                      */
  /*               distance         degrees                            */
  /*                ratio              |                               */
  /*                  |                |                               */
  /*                  |                v                               */
  /*                  v                                                */
  /*    a11  a12              cos a'-a   -sin a'-a                     */
  /*  (          ) = d'/d * (                      )                   */
  /*    a21  a12              sin a'-a    cos a'-a                     */
  /*                                                                   */

  const double d = old1.distanceTo(old2);
  const double a = old1.angleTo(old2);

  const double dp = new1.distanceTo(new2);
  const double ap = new1.angleTo(new2);

  const double diff_a = ap - a;

  const double ratio_d = (dp/d);

  const double cos_diff_a = cos(diff_a);
  const double sin_diff_a = sin(diff_a);

  const double a11 =   ratio_d * cos_diff_a;
  const double a12 = - ratio_d * sin_diff_a;
  const double a21 =   ratio_d * sin_diff_a;
  const double a22 =   ratio_d * cos_diff_a;

  // Loop over all the nodes in between the nodes given by i1 and i2
  for (int n = (i1+1)%itsLength; n != i2; n = (n+1)%itsLength)
    {
      /*                                              */
      /*   x'      c1       a11   a12     x - b1      */
      /* (   ) = (    ) + (           ) (        )    */
      /*   y'      c2       a21   a12     y - b2      */
      /*                                              */

      const double diffx = itsElem[n].x() - old1.x();
      const double diffy = itsElem[n].y() - old1.y();

      itsElem[n].x() = new1.x() + a11*diffx + a12*diffy;
      itsElem[n].y() = new1.y() + a21*diffx + a22*diffy;
    }
}

static const char vcid_snake_cc[] = "$Header$";
#endif // !SNAKE_CC_DEFINED
