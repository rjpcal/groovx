///////////////////////////////////////////////////////////////////////
//
// num.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar  8 16:28:26 2001
// written: Tue Mar 13 12:02:06 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NUM_CC_DEFINED
#define NUM_CC_DEFINED

#include "num.h"

#include "error.h"
#include "mtx.h"

#include "trace.h"

const double Num::SQRT_2 = 1.41421356237;

bool Num::filled = false;
double Num::lookup[TABLE_SIZE] = { 0.0 };

void Num::linearCombo(const ConstSlice& vec,
							 const Mtx& mtx,
							 double* result)
{
DOTRACE("Num::linearCombo");

  // e.g mtx.mrows == vec.nelems == 3   mtx.ncols == 4
  //
  //               | e11  e12  e13  e14 |
  // [w1 w2 w3] *  | e21  e22  e23  e24 | = 
  //               | e31  e32  e33  e34 |
  //
  //
  // [ w1*e11+w2*e21+w3*e31  w1*e12+w2*e22+w3*e32  ... ]

  if (vec.nelems() != mtx.mrows())
	 throw ErrorWithMsg("dimension mismatch in linearCombo");


  for (int col = 0; col < mtx.ncols(); ++col)
	 {
		result[col] = 0.0;
  		for (int row = 0; row < mtx.mrows(); ++row)
  		  {
  			 result[col] += vec[row] * mtx.at(row,col);
		  }
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

static const char vcid_num_cc[] = "$Header$";
#endif // !NUM_CC_DEFINED
