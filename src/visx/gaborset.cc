///////////////////////////////////////////////////////////////////////
//
// gaborset.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 12 11:15:35 2003
// written: Mon May 12 13:25:07 2003
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

#ifndef GABORSET_CC_DEFINED
#define GABORSET_CC_DEFINED

#include "gaborset.h"

#include "gx/geom.h"

#include <cmath>

#include "util/trace.h"

namespace
{
  const double DELTA_THETA    = M_PI / GABOR_MAX_ORIENT;
  const double DELTA_PHASE    = 2 * M_PI / GABOR_MAX_PHASE;

  GaborPatch* createPatch(double sigma, double omega, double theta,
                          double phi, double contrast, int xysize)
  {
  DOTRACE("createPatch");

    GaborPatch* const result = new GaborPatch(xysize);

    const double ssqr  = 2.*sigma*sigma;

    const double cos_theta = cos(theta);
    const double sin_theta = sin(theta);

    double* ptr = result->itsData;

    for (int iy = 0; iy < xysize; ++iy)
      {
        const double fy = iy - xysize / 2.0 + 0.5;

        for (int ix = 0; ix < xysize; ++ix)
          {
            const double fx = ix - xysize / 2.0 + 0.5;

            const double dx = cos_theta * fx - sin_theta * fy;
            const double dy = sin_theta * fx + cos_theta * fy;

            const double dsqr  = (dx*dx + dy*dy) / ssqr;

            const double sinus = cos(omega * dx + phi);

            const double gauss = exp(-dsqr);
            *ptr++ = contrast * sinus * gauss;
          }
      }

    return result;
  }
}

GaborSet::GaborSet(double period, double sigma, int size)
{
DOTRACE("GaborSet::GaborSet");

  const double omega = 2 * M_PI / period;

  for (int n = 0; n < GABOR_MAX_ORIENT; ++n)
    for (int m=0; m < GABOR_MAX_PHASE; ++m)
      {
        Patch[n][m] = createPatch(sigma,
                                  omega,
                                  n * DELTA_THETA,
                                  m * DELTA_PHASE,
                                  /* contrast */ 1.0,
                                  size);
      }
}

GaborSet::~GaborSet()
{
DOTRACE("GaborSet::~GaborSet");

  for (int i=0; i<GABOR_MAX_ORIENT; ++i)
    for (int j=0; j<GABOR_MAX_PHASE; ++j)
      delete Patch[i][j];
}

const GaborPatch& GaborSet::getPatch(double theta, double phi) const
{
DOTRACE("GaborSet::getPatch");

  theta = zerotopi(theta);
  phi = zerototwopi(phi);

  const int itheta = int(theta/DELTA_THETA + 0.5);
  const int iphi   = int(phi/DELTA_PHASE + 0.5);

  return *(Patch[itheta % GABOR_MAX_ORIENT][iphi % GABOR_MAX_PHASE]);
}

static const char vcid_gaborset_cc[] = "$Header$";
#endif // !GABORSET_CC_DEFINED
