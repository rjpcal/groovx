///////////////////////////////////////////////////////////////////////
//
// bezier.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 20 12:17:07 2002
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

static const char vcid_bezier_h[] = "$Header$";
#endif // !BEZIER_H_DEFINED
