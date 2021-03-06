/** @file visx/gaborpatch.h represent the physical parameters of a gabor patch */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon May 12 14:45:43 2003
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_VISX_GABORPATCH_H_UTC20050626084017_DEFINED
#define GROOVX_VISX_GABORPATCH_H_UTC20050626084017_DEFINED

#include "geom/geom.h"

#include <cmath>
#include <cstddef> // size_t

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

  size_t size() const { return itsSize; }

  /// Returns the gabor patch value at the given coordinates.
  /** If the patch has been cached, then we just lookup the value out of
      memory, otherwise we compute it. */
  double at(size_t x, size_t y) const
  {
    return (itsData != nullptr)
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

  double compute(size_t x, size_t y) const;

  const GaborSpec itsSpec;
  const size_t itsSize;
  const double itsCenter;
  const double itsCosTheta;
  const double itsSinTheta;
  const double itsSigmaSqr;
  double* itsData;
};

#endif // !GROOVX_VISX_GABORPATCH_H_UTC20050626084017_DEFINED
