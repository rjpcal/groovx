///////////////////////////////////////////////////////////////////////
//
// num.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar  8 16:27:36 2001
// written: Fri Mar  9 19:01:17 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NUM_H_DEFINED
#define NUM_H_DEFINED

#include <cmath>

class Slice;

class Num {
public:
  static double erfc(double x) {
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

  static const double SQRT_2;

  static void linearCombo(int nelems, const double* w, int w_stride,
								  const Slice* elems, int dim,
								  double* result);

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

static const char vcid_num_h[] = "$Header$";
#endif // !NUM_H_DEFINED
