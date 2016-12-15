/** @file visx/snake.h "snakes" aligned gabor elements */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon May 12 11:15:02 2003
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

#ifndef GROOVX_VISX_SNAKE_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_SNAKE_H_UTC20050626084016_DEFINED

#include "geom/vec2.h"

#include <cstddef> // size_t
#include <vector>

namespace rutz
{
  class urand;
}

struct GaborArrayElement
{
  enum class Type { OUTSIDE, INSIDE, CONTOUR };

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
  Snake(size_t length, double spacing, rutz::urand& urand);
  ~Snake();

  GaborArrayElement getElement(size_t n) const;

private:
  std::vector<geom::vec2<double> > itsElem;

  size_t adjust_index(size_t b, int offset) const
  {
    if (offset > 0)
      return (b + size_t(offset)) % itsElem.size();
    // else
    while (b < size_t(-offset))
      b += itsElem.size();
    return (b - size_t(-offset)) % itsElem.size();
  }

  geom::vec2<double>& elem(size_t b, int offset)
  {
    return itsElem[adjust_index(b, offset)];
  }

  const geom::vec2<double>& elem(size_t b, int offset) const
  {
    return itsElem[adjust_index(b, offset)];
  }

  // Returns true if jiggling converged
  bool jiggle(rutz::urand& urand);
  void transformPath(size_t i1, const geom::vec2<double>& new1,
                     size_t i2, const geom::vec2<double>& new2);
};

#endif // !GROOVX_VISX_SNAKE_H_UTC20050626084016_DEFINED
