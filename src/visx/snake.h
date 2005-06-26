///////////////////////////////////////////////////////////////////////
//
// snake.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon May 12 11:15:02 2003
// commit: $Id$
// $HeadURL$
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

#ifndef SNAKE_H_DEFINED
#define SNAKE_H_DEFINED

#include "geom/vec2.h"

#include "util/arrays.h"

namespace rutz
{
  class urand;
}

struct GaborArrayElement
{
  enum Type { OUTSIDE, INSIDE, CONTOUR };

  GaborArrayElement() {}

  GaborArrayElement(double x, double y, double th, Type t) :
    pos(x,y), theta(th), type(t)
  {}

  GaborArrayElement(const geom::vec2<int>& v, double th, Type t) :
    pos(v), theta(th), type(t)
  {}

  geom::vec2d pos;   // position of node
  double theta;      // angle to x-axis, 0 to TWOPI
  Type type;
};

class Snake
{
public:
  Snake(int l, double spacing, rutz::urand& urand);
  ~Snake();

  GaborArrayElement getElement(int n) const;

private:
  const int itsLength;
  rutz::fixed_block<geom::vec2<double> > itsElem;

  geom::vec2<double>& elem(int i)
  {
    i = i % itsLength;
    if (i < 0) i += itsLength;
    return itsElem[i];
  }

  const geom::vec2<double>& elem(int i) const
  {
    i = i % itsLength;
    if (i < 0) i += itsLength;
    return itsElem[i];
  }

  // Returns true if jiggling converged
  bool jiggle(rutz::urand& urand);
  void transformPath(int i1, const geom::vec2<double>& new1,
                     int i2, const geom::vec2<double>& new2);
};

static const char vcid_snake_h[] = "$Id$ $URL$";
#endif // !SNAKE_H_DEFINED
