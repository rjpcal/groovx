///////////////////////////////////////////////////////////////////////
//
// gaborset.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 12 11:15:35 2003
// written: Mon May 12 14:42:12 2003
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

#include <map>

#include "util/trace.h"

namespace
{
  typedef std::map<GaborSpec, GaborPatch*> MapType;
  MapType theMap;

  const int NUM_THETA = 64;
  const int NUM_PHASE = 8;
  const double DELTA_THETA = M_PI / NUM_THETA;
  const double DELTA_PHASE = 2 * M_PI / NUM_PHASE;
}

GaborSpec::GaborSpec(double s, double o, double t, double p, double c) :
  theta(DELTA_THETA * (int(zerotopi(t)/DELTA_THETA + 0.5) % NUM_THETA)),
  phi(DELTA_PHASE * (int(zerototwopi(p)/DELTA_PHASE + 0.5) % NUM_PHASE)),
  sigma(s),
  omega(o),
  contrast(c)
{}

bool GaborSpec::operator<(const GaborSpec& x) const
{
  if (theta < x.theta) return true;
  else if (theta == x.theta)
    {
      if (phi < x.phi) return true;
      else if (phi == x.phi)
        {
          if (sigma < x.sigma) return true;
          else if (sigma == x.sigma)
            {
              if (omega < x.omega) return true;
              else if (omega == x.omega)
                {
                  return contrast < x.contrast;
                }
            }
        }
    }

  return false;
}

GaborPatch::GaborPatch(const GaborSpec& spec)
  :
  itsSize(int(8*spec.sigma + 0.5)),
  itsData(new double[itsSize*itsSize])
{
DOTRACE("GaborPatch::GaborPatch");

  const double ssqr  = 2.*spec.sigma*spec.sigma;

  const double cos_theta = cos(spec.theta);
  const double sin_theta = sin(spec.theta);

  double* ptr = itsData;

  for (int iy = 0; iy < itsSize; ++iy)
    {
      const double fy = iy - itsSize / 2.0 + 0.5;

      for (int ix = 0; ix < itsSize; ++ix)
        {
          const double fx = ix - itsSize / 2.0 + 0.5;

          const double dx = cos_theta * fx - sin_theta * fy;
          const double dy = sin_theta * fx + cos_theta * fy;

          const double dsqr  = (dx*dx + dy*dy) / ssqr;

          const double sinus = cos(spec.omega * dx + spec.phi);

          const double gauss = exp(-dsqr);
          *ptr++ = spec.contrast * sinus * gauss;
        }
    }
}

const GaborPatch& GaborPatch::lookup(const GaborSpec& spec)
{
DOTRACE("GaborPatch::lookup");

  GaborPatch*& patch = theMap[spec];

  if (patch == 0)
    {
      patch = new GaborPatch(spec);
    }

  return *patch;
}

const GaborPatch& GaborPatch::lookup(double sigma, double omega,
                                     double theta, double phi,
                                     double contrast)
{
  GaborSpec spec(sigma, omega, theta, phi, contrast);

  return lookup(spec);
}

static const char vcid_gaborset_cc[] = "$Header$";
#endif // !GABORSET_CC_DEFINED
