///////////////////////////////////////////////////////////////////////
//
// bezier4.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 20 12:17:39 2002
// written: Wed Nov 20 12:23:02 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BEZIER4_H_DEFINED
#define BEZIER4_H_DEFINED

#include "util/algo.h"

#include <cmath>

///////////////////////////////////////////////////////////////////////
/**
 *
 * The Bezier4 class is specialized for evaluating Bezier curves with
 * four control points.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Bezier4
{
private:
  double R[4];        // control points

  // coefficients of the Bezier polynomial
  // p(u) = c0[0] + c0[1]*u + c0[2]*u^2 + c0[3]*u^3
  double c0[4];

  // coefficients of the derivative polynomial
  // p'(u) = c1[0] + c1[1]*u + c1[2]*u^2
  double c1[4];

  double valMin;
  double valMax;

  bool haveMinMax;

  void computeMinMax();

public:
  /// Construct from four control points.
  Bezier4(double R0, double R1, double R2, double R3);

  /// Reset the control points.
  void setCtrlPnts(double R0, double R1, double R2, double R3);

  /// Evaluate the Bezier curve for a given u in [0, 1]
  double eval(double u);
  /// Evaluate the derivative of the Bezier curve for a given u in [0, 1]
  double evalDeriv(double u);

  /** Returns the maximum value that the Bezier curve will take in the
      range u in [0, 1] */
  double evalMax();
  /** Returns the minimum value that the Bezier curve will take in the
      range u in [0, 1] */
  double evalMin();
};

Bezier4::Bezier4(double R0, double R1, double R2, double R3) :
  R(),
  c0(),
  c1(),
  valMin(0.0),
  valMax(0.0),
  haveMinMax(false)
{
  setCtrlPnts(R0, R1, R2, R3);
}

void Bezier4::computeMinMax()
{
  double b2_4ac = c1[1]*c1[1] - 4*c1[0]*c1[2];

  using Util::max;
  using Util::min;

  valMin = min(eval(0.0), eval(1.0));
  valMax = max(eval(0.0), eval(1.0));

  if (b2_4ac >= 0.0)
    {
      double part1 = -c1[1] / (2*c1[2]);
      double part2 = sqrt(b2_4ac) / (2*c1[2]);

      double extremum1 = Util::clamp(part1 + part2, 0.0, 1.0);
      double extremum2 = Util::clamp(part1 - part2, 0.0, 1.0);

      double v1 = eval(extremum1);
      double v2 = eval(extremum2);

      if (v1 < valMin) valMin = v1; else if (v1 > valMax) valMax = v1;
      if (v2 < valMin) valMin = v2; else if (v2 > valMax) valMax = v2;
    }

  haveMinMax = true;
}

void Bezier4::setCtrlPnts(double R0, double R1, double R2, double R3)
{
  haveMinMax = false;

  R[0] = R0;
  R[1] = R1;
  R[2] = R2;
  R[3] = R3;

  // Compute polynomial coefficients
  c0[0] =    R[0];
  c0[1] = -3*R[0] + 3*R[1];
  c0[2] =  3*R[0] - 6*R[1] + 3*R[2];
  c0[3] =   -R[0] + 3*R[1] - 3*R[2] + R[3];

  // Compute polynomial coefficients of derivative
  c1[0] = 1 * c0[1];
  c1[1] = 2 * c0[2];
  c1[2] = 3 * c0[3];
}

double Bezier4::eval(double u)
{
  return (c0[0] + c0[1]*u + c0[2]*u*u + c0[3]*u*u*u);
}

double Bezier4::evalDeriv(double u)
{
  return (c1[0] + c1[1]*u + c1[2]*u*u);
}

double Bezier4::evalMax()
{
  if (!haveMinMax) computeMinMax();
  return valMax;
}

double Bezier4::evalMin()
{
  if (!haveMinMax) computeMinMax();
  return valMin;
}

static const char vcid_bezier4_h[] = "$Header$";
#endif // !BEZIER4_H_DEFINED
