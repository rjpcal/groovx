///////////////////////////////////////////////////////////////////////
//
// num.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Mar  8 16:27:36 2001
// written: Tue Apr  1 18:04:24 2003
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

#ifndef NUM_H_DEFINED
#define NUM_H_DEFINED

#include <cmath>

/// Numeric operations.
class Num
{
public:
  /// the error function
  static double erfc(double x)
  {
    double z = fabs(x);

    double t = 1.0/(1.0+0.5*fabs(x));

    double ans =
      t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
      t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
      t*(-0.82215223+t*0.17087277)))))))))
      ;

    return x >= 0.0 ? ans : 2.0-ans;
  }

  /// natural-log of the gamma function
  static double gammaln(double xx)
  {
    double tol = 1e-50;

    int ival = int(xx);

    if ( (ival < TABLE_SIZE) && (xx - double(ival) < tol) )
      {
        if (!filled) fillTable();
        return lookup[ival];
      }
    else
      {
        return gammalnEngine(xx);
      }
  }

  /// A speedy version of the exponential function.
  /** Computes the result to the seventh term of the Taylor series. */
  static double fastexp7(double xx);

  static const double SQRT_2;

private:
  static bool filled;
  static const int TABLE_SIZE = 101;
  static double lookup[TABLE_SIZE];

  static void fillTable()
  {
    for (int i = 0; i < TABLE_SIZE; ++i)
      lookup[i] = gammalnEngine(double(i));

    filled = true;
  }

  static double gammalnEngine(double xx);
};

///////////////////////////////////////////////////////////////////////
//
// Functors
//
///////////////////////////////////////////////////////////////////////

/// Functor for "multiply argument by x" operation.
struct Mul
{
  const double factor;                             ///< value to be multiplied

  Mul(double fac) : factor(fac) {}                 ///< construct w/ value
  double operator()(double d) { return d*factor; } ///< function-call operator
};

/// Functor for "divide argument by x" operation.
struct Div : public Mul
{
  Div(double div) : Mul(1.0/div) {}                ///< function-call operator
};

/// Functor for "add x to argument" operation.
struct Add
{
  const double x;                                  ///< value to be added

  Add(double x_) : x(x_) {}                        ///< construct w/ value
  double operator()(double d) { return d + x; }    ///< function-call operator
};

/// Functor for "subtract x from argument" operation.
struct Sub : public Add
{
  Sub(double x_) : Add(-x_) {}                     ///< construct w/ value
};

/// Functor for "argument to power p" operation.
struct ToPow
{
  const double p;                                  ///< power to be applied

  ToPow(double p_) : p(p_) {}                      ///< construct w/ power
  double operator()(double v) { return pow(v, p); }///< function-call operator
};

/// Functor for "square of argument" operation.
struct Square
{
  double operator()(double x) { return x*x; }      ///< function-call operator
};

/// Functor for "minimum of two values" operation.
struct Min
{
  double operator()(double v1, double v2)          ///< function-call operator
  { return (v1 < v2) ? v1 : v2; }
};

/// Functor for "maximum of two values" operation.
struct Max
{
  double operator()(double v1, double v2)          ///< function-call operator
  { return (v1 > v2) ? v1 : v2; }
};

static const char vcid_num_h[] = "$Header$";
#endif // !NUM_H_DEFINED
