///////////////////////////////////////////////////////////////////////
//
// snake.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon May 12 11:15:02 2003
// commit: $Id$
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

#ifndef SNAKE_H_DEFINED
#define SNAKE_H_DEFINED

#include "gx/vec2.h"

#include "util/arrays.h"

namespace Util
{
  class Urand;
}

struct Element
{
  enum Type { OUTSIDE, INSIDE, CONTOUR };

  Element() {}

  Element(double x, double y, double th, Type t) :
    pos(x,y), theta(th), type(t)
  {}

  Element(const Gfx::Vec2<int>& v, double th, Type t) :
    pos(v), theta(th), type(t)
  {}

  Gfx::Vec2d pos;    // position of node
  double theta;      // angle to x-axis, 0 to TWOPI
  Type type;
};

class Snake
{
public:
  Snake(int l, double spacing, Util::Urand& urand);
  ~Snake();

  Element getElement(int n) const;

private:
  const int itsLength;
  const double itsSpacing;
  fixed_block<Gfx::Vec2<double> > itsElem;

  Gfx::Vec2<double>& elem(int i)
  {
    i = i % itsLength;
    if (i < 0) i += itsLength;
    return itsElem[i];
  }

  const Gfx::Vec2<double>& elem(int i) const
  {
    i = i % itsLength;
    if (i < 0) i += itsLength;
    return itsElem[i];
  }

  // Returns true if jiggling converged
  bool jiggle(Util::Urand& urand);
  void transformPath(int i1, const Gfx::Vec2<double>& new1,
                     int i2, const Gfx::Vec2<double>& new2);
};

static const char vcid_snake_h[] = "$Header$";
#endif // !SNAKE_H_DEFINED
