/** @file pkgs/mtx/arithfunctor.h arithmetic functors that can be
    applied to mtx objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Mar  8 16:27:36 2001 (split from num.h)
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

#ifndef GROOVX_PKGS_MTX_ARITHFUNCTOR_H_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_ARITHFUNCTOR_H_UTC20050626084022_DEFINED

#include <cmath>

namespace dash
{
  /// Functor for "multiply argument by x" operation.
  struct mul
  {
    const double factor;                             ///< value to be multiplied

    mul(double fac) : factor(fac) {}                 ///< construct w/ value
    double operator()(double d) { return d*factor; } ///< function-call operator
  };

  /// Functor for "divide argument by x" operation.
  struct div : public mul
  {
    div(double div) : mul(1.0/div) {}                ///< function-call operator
  };

  /// Functor for "add x to argument" operation.
  struct add
  {
    const double x;                                  ///< value to be added

    add(double x_) : x(x_) {}                        ///< construct w/ value
    double operator()(double d) { return d + x; }    ///< function-call operator
  };

  /// Functor for "subtract x from argument" operation.
  struct sub : public add
  {
    sub(double x_) : add(-x_) {}                     ///< construct w/ value
  };

  /// Functor for "argument to power p" operation.
  struct to_power
  {
    const double p;                                  ///< power to be applied

    to_power(double p_) : p(p_) {}                   ///< construct w/ power
    double operator()(double v) { return pow(v, p); }///< function-call operator
  };

  /// Functor for "square of argument" operation.
  struct square
  {
    double operator()(double x) { return x*x; }      ///< function-call operator
  };

  /// Functor for "minimum of two values" operation.
  struct min
  {
    double operator()(double v1, double v2)          ///< function-call operator
    { return (v1 < v2) ? v1 : v2; }
  };

  /// Functor for "maximum of two values" operation.
  struct max
  {
    double operator()(double v1, double v2)          ///< function-call operator
    { return (v1 > v2) ? v1 : v2; }
  };

  /// Functor for "set all elements to x" operation.
  struct setter
  {
    const double v;
    setter(double v_) : v(v_) {}
    double operator()(double) { return v; }
  };
}

#endif // !GROOVX_PKGS_MTX_ARITHFUNCTOR_H_UTC20050626084022_DEFINED
