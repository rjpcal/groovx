///////////////////////////////////////////////////////////////////////
//
// gaborarray.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon May 12 11:15:51 2003
// written: Mon May 12 13:33:13 2003
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

#include "gx/bmapdata.h"

#include "util/arrays.h"

#include "visx/gaborset.h"
#include "visx/snake.h"

namespace Gfx
{
  class BmapData;
  template <class T> class Vec2;
}

class GaborArray : public GxShapeKit
{
public:
  GaborArray(double gaborPeriod = 15.0, double gaborSigma = 7.5,
             int foregNumber = 24, double foregSpacing = 45.0,
             int sizeX_ = 512, int sizeY_ = 512,
             double backgIniSpacing_ = 48.0,
             double backgMinSpacing_ = 36.0);

  static GaborArray* make() { return new GaborArray; }

  void renderInto(Gfx::BmapData& data) const;

  void saveImage(const char* filename) const;

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const;

  virtual void grRender(Gfx::Canvas& canvas) const;

private:
  const Snake snake;
  const GaborSet gabors;
  const int sizeX;
  const int sizeY;
  const double halfX;
  const double halfY;
  const double backgIniSpacing;
  const double backgMinSpacing;
  const double backgMinSpacingSqr;
  int insideNumber;
  int totalNumber;
  fixed_block<Element> array;

  void dumpFrame() const;

  bool tryPush(const Element& e);
  bool tooClose(const Gfx::Vec2<double>& v, int except);
  void insideElements();
  void hexGridElements();
  void fillElements();
  void jitterElement();
};

static const char vcid_gaborarray_h[] = "$Header$";
#endif // !GABORARRAY_H_DEFINED
