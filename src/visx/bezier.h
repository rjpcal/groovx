///////////////////////////////////////////////////////////////////////
//
// bezier.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 21 09:51:40 1999
// written: Tue Sep 21 11:04:05 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BEZIER_H_DEFINED
#define BEZIER_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef CMATH_DEFINED
#include <cmath>
#define CMATH_DEFINED
#endif

class Bezier4 {
private:
  vector<double> R;				  // control points

  // coefficients of the Bezier polynomial
  // p(u) = c0[0] + c0[1]*u + c0[2]*u^2 + c0[3]*u^3
  vector<double> c0;

  // coefficients of the derivative polynomial
  // p'(u) = c1[0] + c1[1]*u + c1[2]*u^2
  vector<double> c1;

  double b2_4ac;

  double extremum1;
  double extremum2;

public:
  Bezier4(double R0, double R1, double R2, double R3);

  void setCtrlPnts(double R0, double R1, double R2, double R3);

  double eval(double u);
  double evalDeriv(double u);

  double evalMax();
  double evalMin();
};

Bezier4::Bezier4(double R0, double R1, double R2, double R3) :
  R(4),
  c0(4),
  c1(3)
{
  setCtrlPnts(R0, R1, R2, R3);
}

void Bezier4::setCtrlPnts(double R0, double R1, double R2, double R3) {
  R[0] = R0;
  R[1] = R1;
  R[2] = R2;
  R[3] = R3;

  c0[0] =    R[0];
  c0[1] = -3*R[0] + 3*R[1];
  c0[2] =  3*R[0] - 6*R[1] + 3*R[2];
  c0[3] =   -R[0] + 3*R[1] - 3*R[2] + R[3];

  for (int i = 0; i < c1.size(); ++i) {
	 c1[i] = (i+1) * c0[i+1];
  }

  b2_4ac = c1[1]*c1[1] - 4*c1[0]*c1[2];

  if (b2_4ac >= 0.0) {
	 double part1 = -c1[1] / (2*c1[2]);
	 double part2 = sqrt(b2_4ac) / (2*c1[2]);

	 extremum1 = part1 + part2;
	 if (extremum1 < 0.0) extremum1 = 0.0;
	 if (extremum1 > 1.0) extremum1 = 1.0;

	 extremum2 = part1 - part2;
	 if (extremum2 < 0.0) extremum2 = 0.0;
	 if (extremum2 > 1.0) extremum2 = 1.0;
  }
  else {
	 extremum1 = 0.0;
	 extremum2 = 1.0;
  }
}

double Bezier4::eval(double u) {
  return (c0[0] + c0[1]*u + c0[2]*u*u + c0[3]*u*u*u);
}

double Bezier4::evalDeriv(double u) {
  return (c1[0] + c1[1]*u + c1[2]*u*u);
}

double Bezier4::evalMax() {
  return max( max(eval(0.0), eval(1.0)),
				  max(eval(extremum1), eval(extremum2)) );
}

double Bezier4::evalMin() {
  return min( min(eval(0.0), eval(1.0)),
				  min(eval(extremum1), eval(extremum2)) );
}

static const char vcid_bezier_h[] = "$Header$";
#endif // !BEZIER_H_DEFINED
