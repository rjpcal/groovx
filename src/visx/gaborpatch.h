///////////////////////////////////////////////////////////////////////
//
// gaborpatch.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon May 12 14:45:43 2003
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

#ifndef GABORPATCH_H_DEFINED
#define GABORPATCH_H_DEFINED

#include "geom/geom.h"

#include <cmath>

struct GaborSpec
{
  /// Construct with given values.
  /** Some values may be discretized into a (smallish) finite number of
      possible values to minimize the total number of GaborPatch objects
      that have to be created. */
  GaborSpec(double s, double o, double t, double p);

  /// Comparison operator for sorting and associative arrays.
  bool operator<(const GaborSpec& x) const;

  const double theta;     ///< Orientation of primary axis
  const double phi;       ///< Phase angle of grating
  const double sigma;     ///< Std dev of gaussian mask
  const double omega;     ///< Spatial frequency of grating
};

/// Manages a pixmap representation of a gabor patch.
/** Can either compute values directly, or can cache the entire patch. */
class GaborPatch
{
public:
  GaborPatch(const GaborSpec& spec);

  ~GaborPatch();

  static const GaborPatch& lookup(const GaborSpec& s);

  static const GaborPatch& lookup(double sigma, double omega,
                                  double theta, double phi);

  int size() const { return itsSize; }

  /// Returns the gabor patch value at the given coordinates.
  /** If the patch has been cached, then we just lookup the value out of
      memory, otherwise we compute it. */
  double at(int x, int y) const
  {
    return (itsData != 0)
      ? itsData[x + y*itsSize]
      : compute(x, y);
  }

  /// Force the entire patch to be cached.
  /** As usual, this is a space-time tradeoff... using the cache will make
      at() lookups faster, but will obviously expend more memory to do
      so. */
  void fillCache();

private:
  GaborPatch(const GaborPatch&);
  GaborPatch& operator=(const GaborPatch&);

  double compute(int x, int y) const;

  const GaborSpec itsSpec;
  const int itsSize;
  const double itsCenter;
  const double itsCosTheta;
  const double itsSinTheta;
  const double itsSigmaSqr;
  double* itsData;
};

static const char vcid_gaborpatch_h[] = "$Id$ $URL$";
#endif // !GABORPATCH_H_DEFINED
