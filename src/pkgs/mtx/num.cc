///////////////////////////////////////////////////////////////////////
//
// num.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Mar  8 16:28:26 2001
// written: Wed Mar 19 12:45:48 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NUM_CC_DEFINED
#define NUM_CC_DEFINED

#include "num.h"

#include "util/trace.h"

const double Num::SQRT_2 = 1.41421356237;

bool Num::filled = false;
double Num::lookup[TABLE_SIZE] = { 0.0 };

namespace
{
  double fastexpImpl(double xx)
  {
    static const double one_over[] = {
      1.0, /* 1/0 -- undefined, really */
      1.0,
      0.5,
      0.333333333333333333333333,
      0.25,
      0.2,
      0.166666666666666666666667,
      0.142857142857142857142857,
      0.125,
      0.111111111111111111111111,
      0.1
    };

    return
      1.0 + (xx * one_over[1])
      * (1.0 + (xx * one_over[2])
         * (1.0 + (xx * one_over[3])
            * (1.0 + (xx * one_over[4])
               * (1.0 + (xx * one_over[5])
                  * (1.0 + (xx * one_over[6])
                     * (1.0 + (xx * one_over[7])
                        )
                     )
                  )
               )
            )
         )
      ;
  }
}

double Num::gammalnEngine(double xx)
{
DOTRACE("Num::gammalnEngine");

  static double cof[6] = {76.18009172947146,     -86.50532032941677,
                          24.01409824083091,     -1.231738572450155,
                          0.1208650973866179e-2, -0.5395239384953e-5};
  double x,y,tmp,ser;
  int j;

  y = x = xx;
  tmp = x+5.5;
  tmp -= (x+0.5)*log(tmp);
  ser=1.000000000190015;
  for (j=0;j<=5;j++) ser += cof[j]/++y;
  return -tmp+log(2.5066282746310005*ser/x);
}

double Num::fastexp7(double x)
{
  return x < 0 ? 1.0/fastexpImpl(-x) : fastexpImpl(x);
}

static const char vcid_num_cc[] = "$Header$";
#endif // !NUM_CC_DEFINED
