///////////////////////////////////////////////////////////////////////
//
// gaborset.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 12 11:15:29 2003
// written: Mon May 12 14:25:37 2003
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

#ifndef GABORSET_H_DEFINED
#define GABORSET_H_DEFINED

#include "gx/geom.h"

#include <cmath>

struct GaborSpec
{
  /// Construct with given values.
  /** Some values may be discretized into a (smallish) finite number of
      possible values to minimize the total number of GaborPatch objects
      that have to be created. */
  GaborSpec(double s, double o, double t, double p, double c);

  /// Comparison operator for sorting and associative arrays.
  bool operator<(const GaborSpec& x) const;

  const double theta;     ///< Orientation of primary axis
  const double phi;       ///< Phase angle of grating
  const double sigma;     ///< Std dev of gaussian mask
  const double omega;     ///< Spatial frequency of grating
  const double contrast;  ///< [0..1]
};

class GaborPatch
{
public:
  GaborPatch(const GaborSpec& spec);

  ~GaborPatch() { delete [] itsData; }

  static const GaborPatch& lookup(const GaborSpec& s);

  static const GaborPatch& lookup(double sigma, double omega,
                                  double theta, double phi, double contrast);

  int size() const { return itsSize; }

  double operator[](int i) const { return itsData[i]; }

  double at(int x, int y) const { return itsData[x + y*itsSize]; }

private:
  const int itsSize;
  double* const itsData;

  GaborPatch(const GaborPatch&);
  GaborPatch& operator=(const GaborPatch&);
};

static const char vcid_gaborset_h[] = "$Header$";
#endif // !GABORSET_H_DEFINED
