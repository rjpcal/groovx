///////////////////////////////////////////////////////////////////////
//
// num.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar  8 16:28:26 2001
// written: Tue Mar 13 12:25:24 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NUM_CC_DEFINED
#define NUM_CC_DEFINED

#include "num.h"

#include "mtx.h"

#include "trace.h"

const double Num::SQRT_2 = 1.41421356237;

bool Num::filled = false;
double Num::lookup[TABLE_SIZE] = { 0.0 };

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

static const char vcid_num_cc[] = "$Header$";
#endif // !NUM_CC_DEFINED
