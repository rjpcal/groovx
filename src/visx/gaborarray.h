/** @file visx/gaborarray.h spatial array of gabor patches, possibly
    with embedded contours ("snakes") */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon May 12 11:15:51 2003
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

#ifndef GROOVX_VISX_GABORARRAY_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_GABORARRAY_H_UTC20050626084016_DEFINED

#include "gfx/gxshapekit.h"
#include "gfx/gbvec.h"

#include "media/bmapdata.h"

#include <memory>
#include <vector>

struct GaborArrayElement;
class Snake;

namespace rutz
{
  class urand;
}

template <class T>
class Cached
{
public:
  Cached(const T& v) noexcept : val(v), oldval(v), changed(true) {}

  operator       T&()       noexcept { return val; }
  operator const T&() const noexcept { return val; }

  void save()  const noexcept { oldval = val; changed = false; }
  bool ok()    const noexcept { return (val == oldval) && !changed; }
  void touch() const noexcept { changed = true; }

  T val;
  mutable T oldval;
  mutable bool changed;
};

/// GaborArray represents an 2-D spatial array of gabor patches.
/** The array has manipulable spacing properties. */
class GaborArray : public GxShapeKit
{
public:
  GaborArray(double gaborPeriod = 15.0, double gaborSigma = 7.5,
             size_t foregNumber = 24u, double foregSpacing = 45.0,
             size_t sizeX = 512u, size_t sizeY = 512u,
             double gridSpacing = 48.0,
             double minSpacing = 36.0);

  virtual ~GaborArray() noexcept;

  static GaborArray* make() { return new GaborArray; }

  /// Get GaborArray's fields.
  static const FieldMap& classFields();

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  void saveImage(const char* filename) const;

  void saveContourOnlyImage(const char* filename) const;

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const override;

  virtual void grRender(Gfx::Canvas& canvas) const override;

private:
  GaborArray(const GaborArray&);
  GaborArray& operator=(const GaborArray&);

  media::bmap_data generateBmap(bool doTagLast = false) const;

  void updateForeg() const;
  void updateBackg() const;
  void updateBmap() const;
  void update() const;
  bool tryPush(const GaborArrayElement& e) const;
  bool tooClose(const geom::vec2<double>& v, size_t except) const;
  void backgHexGrid() const;
  void backgFill() const;
  void backgJitter(rutz::urand& urand) const;

  Cached<unsigned long> itsForegSeed;
  Cached<size_t> itsForegNumber;
  Cached<double> itsForegSpacing;
  Cached<int> itsForegPosX;
  Cached<int> itsForegPosY;

  Cached<unsigned long> itsBackgSeed;
  Cached<size_t> itsSizeX;
  Cached<size_t> itsSizeY;
  Cached<double> itsGridSpacing;
  Cached<double> itsMinSpacing;
  Cached<double> itsFillResolution;

  Cached<unsigned long> itsThetaSeed;
  Cached<unsigned long> itsPhaseSeed;
  Cached<double> itsThetaJitter;
  Cached<double> itsGaborPeriod;
  Cached<double> itsGaborSigma;
  Cached<unsigned long> itsContrastSeed;
  Cached<double> itsContrastJitter;

  mutable std::vector<GaborArrayElement> itsArray;
  mutable media::bmap_data itsBmap;

  bool itsDumpingFrames;
  unsigned int itsFrameDumpPeriod;
};

#endif // !GROOVX_VISX_GABORARRAY_H_UTC20050626084016_DEFINED
