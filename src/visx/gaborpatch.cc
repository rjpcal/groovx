///////////////////////////////////////////////////////////////////////
//
// gaborpatch.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 12 14:45:52 2003
// written: Wed May 14 14:55:24 2003
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

#ifndef GABORPATCH_CC_DEFINED
#define GABORPATCH_CC_DEFINED

#include "gaborpatch.h"

#include <map>

#include "util/trace.h"

using namespace Geom;

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
  theta(DELTA_THETA * (int(rad_0_pi(t)/DELTA_THETA + 0.5) % NUM_THETA)),
  phi(DELTA_PHASE * (int(rad_0_2pi(p)/DELTA_PHASE + 0.5) % NUM_PHASE)),
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
  itsSpec      ( spec ),
  itsSize      ( int(8*spec.sigma + 0.5) ),
  itsCenter    ( itsSize/2.0 + 0.5 ),
  itsCosTheta  ( cos(spec.theta) ),
  itsSinTheta  ( sin(spec.theta) ),
  itsSigmaSqr  ( 2.0* spec.sigma * spec.sigma ),
  itsData      ( 0 )
{
DOTRACE("GaborPatch::GaborPatch");
}

GaborPatch::~GaborPatch()
{
DOTRACE("GaborPatch::~GaborPatch");
  delete [] itsData;
}

const GaborPatch& GaborPatch::lookup(const GaborSpec& spec)
{
DOTRACE("GaborPatch::lookup");

  GaborPatch*& patch = theMap[spec];

  if (patch == 0)
    {
      patch = new GaborPatch(spec);
      patch->fillCache();
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

void GaborPatch::fillCache()
{
  if (itsData == 0)
    {
      itsData = new double[itsSize*itsSize];

      double* ptr = itsData;

      for (int y = 0; y < itsSize; ++y)
        for (int x = 0; x < itsSize; ++x)
          *ptr++ = compute(x, y);
    }
}


double GaborPatch::compute(int x, int y) const
{
  const double fy = y - itsCenter;
  const double fx = x - itsCenter;

  const double dx = itsCosTheta * fx - itsSinTheta * fy;
  const double dy = itsSinTheta * fx + itsCosTheta * fy;

  const double dsqr  = (dx*dx + dy*dy) / itsSigmaSqr;

  const double sinus = cos(itsSpec.omega * dx + itsSpec.phi);

  const double gauss = exp(-dsqr);
  return itsSpec.contrast * sinus * gauss;
}

static const char vcid_gaborpatch_cc[] = "$Header$";
#endif // !GABORPATCH_CC_DEFINED
