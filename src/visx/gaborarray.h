///////////////////////////////////////////////////////////////////////
//
// gaborarray.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 12 11:15:51 2003
// written: Tue May 13 11:14:10 2003
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

/// GaborArray represents an 2-D spatial array of gabor patches.
/** The array has manipulable spacing properties. */
class GaborArray : public GxShapeKit
{
public:
  GaborArray(double gaborPeriod_ = 15.0, double gaborSigma_ = 7.5,
             int foregNumber = 24, double foregSpacing = 45.0,
             int sizeX = 512, int sizeY = 512,
             double backgIniSpacing_ = 48.0,
             double backgMinSpacing_ = 36.0);

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

  void killCache();
  void update() const;
  bool tryPush(const Element& e) const;
  bool tooClose(const Gfx::Vec2<double>& v, int except) const;
  /// Mark inside elements and return how many there were.
  int insideElements(const Snake& snake) const;
  void hexGridElements() const;
  void fillElements() const;
  void jitterElement() const;

  int itsForegNumber;
  double itsForegSpacing;
  GbVec2<int> itsSize;
  double gaborPeriod;
  double gaborSigma;
  double backgIniSpacing;
  double backgMinSpacing;
  mutable int totalNumber;
  mutable fixed_block<Element> array;
  mutable shared_ptr<Gfx::BmapData> itsBmap;
};

static const char vcid_gaborarray_h[] = "$Header$";
#endif // !GABORARRAY_H_DEFINED
