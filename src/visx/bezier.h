///////////////////////////////////////////////////////////////////////
//
// bezier.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 21 09:51:40 1999
// written: Wed Nov 20 12:13:18 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BEZIER_H_DEFINED
#define BEZIER_H_DEFINED

#include "util/algo.h"
#include "util/arrays.h"
#include "util/slink_list.h"

#include <cmath>

///////////////////////////////////////////////////////////////////////
/**
 *
 * Bezier is a general class for evaluating Bezier curves, their
 * derivatives, and their minimum and maximum values.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Bezier
{
private:
  dynamic_block<double> R;      // control points

  // coefficients of the Bezier polynomial
  // p(u) = c0[0] + c0[1]*u + c0[2]*u^2 + c0[3]*u^3
  dynamic_block<double> c0;

  // coefficients of the derivative polynomial
  // p'(u) = c1[0] + c1[1]*u + c1[2]*u^2
  dynamic_block<double> c1;

  double uMin_;    // value of u in [0,1] which gives the minimum
  double uMax_;  // value of u in [0,1] which gives the maximum
  double valMin_;
  double valMax_;

  static int choose(int n, int k)
  {
    // result = n! / [k!(n-k)!]
    int result = 1;

    // we have symmetry of (n k) = (n n-k)
    // smaller k gives a faster computation, so:
    if ( (n-k) < k ) { k = n-k; }

    // compute n! / (n-k)!  =  n*(n-1)*(n-2)...(n-k+1)
    for (int f = n; f > (n-k); --f)
      {
        result *= f;
      }
    for (int d = 2; d <= k; ++d)
      {
        result /= d;
      }

    return result;
  }

public:
  /** Construct from a vector of control points. The argument
      extrema_res is used to specify the number of subdivisions that
      will be examined when searching for the extrema. If extrema_res
      is < 0 (the default value), a default number of subdivisions
      will be used. */
  Bezier(const dynamic_block<double>& RR, int extrema_res=-1);

  /// Reset the control points,
  void setCtrlPnts(const dynamic_block<double>& RR, int extrema_res=-1);


  /// Evaluate the Bezier curve for a given u in [0, 1]
  double eval(double u);
  /// Evaluate the derivative of the Bezier curve for a given u in [0, 1]
  double evalDeriv(double u);

  /** Returns the value of u at which the Bezier curve takes its
      maximum value. */
  double uMin() { return uMin_; }
  /** Returns the value of u at which the Bezier curve takes its
      minimum value. */
  double uMax() { return uMax_; }

  /** Returns the maximum value that the Bezier curve will take in the
      range u in [0, 1] */
  double valMin() { return valMin_; }
  /** Returns the minimum value that the Bezier curve will take in the
      range u in [0, 1] */
  double valMax() { return valMax_; }
};

Bezier::Bezier(const dynamic_block<double>& RR, int extrema_res) :
  R(),
  c0(),
  c1(),
  uMin_(0.0),
  uMax_(0.0),
  valMin_(0.0),
  valMax_(0.0)
{
  setCtrlPnts(RR, extrema_res);
}

void Bezier::setCtrlPnts(const dynamic_block<double>& RR, int extrema_res)
{
  R = RR;
  c0.resize(RR.size());
  c1.resize(RR.size()-1);

  int n = RR.size()-1;          // degree of polynomial = num_points - 1

  //         ( n )    i  {     (i+k)   ( i )      }
  // c0    = (   ) * sum {  (-1)     * (   ) * R  }
  //   i     ( i )   k=0 {             ( k )    k }

  {for (int i = 0; i <= n; ++i)
    {
      c0[i] = 0;

      for (int k = 0; k <= i; ++k)
        {
          int i_choose_k = Bezier::choose(i,k);

          c0[i] += ( (i+k)%2 ? 1 : -1) * i_choose_k * R[k];
        }

      c0[i] *= Bezier::choose(n,i);
    }
  }

  {for (size_t i = 0; i < c1.size(); ++i)
    {
      c1[i] = (i+1) * c0[i+1];
    }
  }


  if (extrema_res <= 0) { extrema_res = 4*(RR.size()); }

  slink_list<double> extrema;

  extrema.push_front(0.0);

  double prev, current = evalDeriv(0.0);
  for (int e = 1; e <= extrema_res; ++e)
    {
      prev = current;
      current = evalDeriv(double(e)/extrema_res);
      // See if the derivative has crossed zero by checking if the signs
      // of current and prev are different
      if ( (prev > 0.0) != (current > 0.0) )
        {
          extrema.push_front((current+prev)/2.0);
        }
    }

  extrema.push_front(1.0);

  uMin_ = uMax_ = 0.0;
  valMin_ = valMax_ = eval(0.0);

  for (slink_list<double>::iterator
         ii = extrema.begin(),
         end = extrema.end();
       ii != end;
       ++ii)
    {
      double current = eval(*ii);
      if (current < valMin_)
        {
          valMin_ = current;
          uMin_ = *ii;
        }
      if (current > valMax_)
        {
          valMax_ = current;
          uMax_ = *ii;
        }
    }
}

double Bezier::eval(double u)
{
  double result = 0.0;
  double powers = 1.0;
  for (size_t i = 0; i < c0.size(); ++i)
    {
      result += c0[i] * powers;
      powers *= u;
    }
  return result;
}

double Bezier::evalDeriv(double u)
{
  double result = 0.0;
  double powers = 1.0;
  for (size_t i = 0; i < c1.size(); ++i)
    {
      result += c1[i] * powers;
      powers *= u;
    }
  return result;
}

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

static const char vcid_bezier_h[] = "$Header$";
#endif // !BEZIER_H_DEFINED
