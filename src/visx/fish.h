///////////////////////////////////////////////////////////////////////
//
// fish.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 11:44:56 1999
// written: Thu Sep 30 12:09:47 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISH_H_DEFINED
#define FISH_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef GROBJ_H_DEFINED
#include "grobj.h"
#endif

#ifndef PROPERTY_H_DEFINED
#include "property.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// Fish class definition
//
///////////////////////////////////////////////////////////////////////

class Fish : public GrObj, public PropFriend<Fish> {
public:
  Fish(const char* splinefile, const char* coordfile, int index);

private:
  void read_splinefile(const char* splinefile);
  void read_coordfile(const char* coordfile, int index);

public:
  virtual ~Fish();

  virtual void serialize(ostream &os, IOFlag flag) const {}
  virtual void deserialize(istream &is, IOFlag flag) {}
  // These functions write the object's state from/to an output/input
  // stream. Both functions are defined, but are no-ops for GrObj.

  virtual int charCount() const { return 0; }

  //////////////////
  // nested types //
  //////////////////

  //fish structure: describes one of the part of the fish (upper fin,
  //tail, bottom fin or mouth). The descripition is in terms of a
  //spline curve, with given order, number of knots and x,y
  //coefficients.
  struct FISH_PART {
	 int order;
	 
	 int nknots;
	 float *knots;

	 int ncoefs;
	 float *coefs[2];
  };

  //endpoints structure: Each of the endpoint (four in total) is
  //associated with a breakpoint of one of the parts. This is
  //described in this structure as well as the x and y coordinates of
  //the two points which define the boundaries of the endpoint line
  struct ENDPT {
	 int part;
	 int bkpt;
	 float x[2];
	 float y[2];
  };

  ////////////////
  // properties //
  ////////////////

  /////////////
  // actions //
  /////////////

protected:
  virtual void grRender() const;

private:
  FISH_PART fish[4];
  ENDPT endpts[4];
  float fishcoord[4];
};

static const char vcid_fish_h[] = "$Header$";
#endif // !FISH_H_DEFINED


