///////////////////////////////////////////////////////////////////////
//
// num.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Mar  8 16:27:36 2001
// written: Wed Mar 19 12:45:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NUM_H_DEFINED
#define NUM_H_DEFINED

#include <cmath>

/// Numeric operations.
class Num
{
public:
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

  // This is a speedy version of the exponential function, which
  // computes the result to the seventh term of the Taylor series.
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
  const double factor;

  Mul(double fac) : factor(fac) {}
  double operator()(double d) { return d*factor; }
};

/// Functor for "divide argument by x" operation.
struct Div : public Mul
{
  Div(double div) : Mul(1.0/div) {}
};

/// Functor for "add x to argument" operation.
struct Add
{
  const double x;

  Add(double x_) : x(x_) {}
  double operator()(double d) { return d + x; }
};

/// Functor for "subtract x from argument" operation.
struct Sub : public Add
{
  Sub(double x_) : Add(-x_) {}
};

/// Functor for "argument to power p" operation.
struct ToPow
{
  const double p;

  ToPow(double p_) : p(p_) {}
  double operator()(double v) { return pow(v, p); }
};

/// Functor for "square of argument" operation.
struct Square
{
  double operator()(double x) { return x*x; }
};

/// Functor for "minimum of two values" operation.
struct Min
{
  double operator()(double v1, double v2)
  { return (v1 < v2) ? v1 : v2; }
};

/// Functor for "maximum of two values" operation.
struct Max
{
  double operator()(double v1, double v2)
  { return (v1 > v2) ? v1 : v2; }
};

static const char vcid_num_h[] = "$Header$";
#endif // !NUM_H_DEFINED
