///////////////////////////////////////////////////////////////////////
//
// num.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar  8 16:27:36 2001
// written: Fri Jan 18 16:06:51 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NUM_H_DEFINED
#define NUM_H_DEFINED

#include <cmath>

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

struct Mul
{
  const double factor;

  Mul(double fac) : factor(fac) {}
  double operator()(double d) { return d*factor; }
};

struct Div : public Mul
{
  Div(double div) : Mul(1.0/div) {}
};

struct Add
{
  const double x;

  Add(double x_) : x(x_) {}
  double operator()(double d) { return d + x; }
};

struct Sub : public Add
{
  Sub(double x_) : Add(-x_) {}
};

static const char vcid_num_h[] = "$Header$";
#endif // !NUM_H_DEFINED