///////////////////////////////////////////////////////////////////////
//
// gaborarray.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 12 11:15:51 2003
// written: Wed May 14 18:04:24 2003
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

#ifndef GABORARRAY_H_DEFINED
#define GABORARRAY_H_DEFINED

#include "gfx/gxshapekit.h"

#include "gfx/gbvec.h"

#include "util/arrays.h"
#include "util/pointers.h"

class Element;
class Snake;

namespace Gfx
{
  class BmapData;
}

namespace Util
{
  class Urand;
}

template <class T>
class Cached
{
public:
  Cached(const T& v) throw() : val(v), oldval(v), changed(true) {}

  operator       T&()       throw() { return val; }
  operator const T&() const throw() { return val; }

  void save()  const throw() { oldval = val; changed = false; }
  bool ok()    const throw() { return (val == oldval) && !changed; }
  void touch() const throw() { changed = true; }

  T val;
  mutable T oldval;
  mutable bool changed;
};

namespace Util
{
  template <class T>
  struct TypeTraits<Cached<T> > : public TypeTraits<T>
  {};
}

/// GaborArray represents an 2-D spatial array of gabor patches.
/** The array has manipulable spacing properties. */
class GaborArray : public GxShapeKit
{
public:
  GaborArray(double gaborPeriod = 15.0, double gaborSigma = 7.5,
             int foregNumber = 24, double foregSpacing = 45.0,
             int sizeX = 512, int sizeY = 512,
             double gridSpacing = 48.0,
             double minSpacing = 36.0);

  static GaborArray* make() { return new GaborArray; }

  /// Get GaborArray's fields.
  static const FieldMap& classFields();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  void saveImage(const char* filename) const;

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const;

  virtual void grRender(Gfx::Canvas& canvas) const;

private:
  GaborArray(const GaborArray&);
  GaborArray& operator=(const GaborArray&);

  void dumpFrame() const;

  void updateForeg() const;
  void updateBackg() const;
  void updateBmap() const;
  void update() const;
  bool tryPush(const Element& e) const;
  bool tooClose(const Gfx::Vec2<double>& v, int except) const;
  void backgHexGrid() const;
  void backgFill() const;
  void backgJitter(Util::Urand& urand) const;

  Cached<int> itsForegSeed;
  Cached<int> itsForegNumber;
  Cached<double> itsForegSpacing;
  Cached<int> itsForegPosX;
  Cached<int> itsForegPosY;

  Cached<int> itsBackgSeed;
  Cached<int> itsSizeX;
  Cached<int> itsSizeY;
  Cached<double> itsGridSpacing;
  Cached<double> itsMinSpacing;

  Cached<int> itsThetaSeed;
  Cached<int> itsPhaseSeed;
  Cached<double> itsThetaJitter;
  Cached<double> itsGaborPeriod;
  Cached<double> itsGaborSigma;
  Cached<int> itsContrastSeed;
  Cached<double> itsContrastJitter;

  mutable int itsTotalNumber;
  mutable fixed_block<Element> itsArray;
  mutable shared_ptr<Gfx::BmapData> itsBmap;
};

static const char vcid_gaborarray_h[] = "$Header$";
#endif // !GABORARRAY_H_DEFINED
