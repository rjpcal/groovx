///////////////////////////////////////////////////////////////////////
//
// fish.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 11:44:57 1999
// written: Sat Oct  2 21:24:17 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISH_CC_DEFINED
#define FISH_CC_DEFINED

#include "fish.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>

#include "error.h"

#define LOCAL_TRACE
#include "trace.h"
#define LOCAL_DEBUG
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// FishPart struct --
//
// Describes one of the part of the fish (upper fin, tail, bottom fin
// or mouth). The descripition is in terms of a spline curve, with
// given order, number of knots and x,y coefficients.
//
///////////////////////////////////////////////////////////////////////

struct Fish::FishPart {
  GLint itsOrder;

  vector<GLfloat> itsKnots;

  vector<GLfloat> itsCoefs[2];
};

/////////////////////////////////////////////////////////////////////////
//
// EndPt struct --
//
// Each of the endpoint (four in total) is associated with a
// breakpoint of one of the parts. This is described in this structure
// as well as the x and y coordinates of the two points which define
// the boundaries of the endpoint line
//
///////////////////////////////////////////////////////////////////////

struct Fish::EndPt {
  int itsPart;
  int itsBkpt;
  float itsX[2];
  float itsY[2];
};


///////////////////////////////////////////////////////////////////////
//
// Fish member definitions
//
///////////////////////////////////////////////////////////////////////

Fish::Fish(const char* splinefile, const char* coordfile, int index) :
  currentPart(0),
  currentEndPt(0),
  coord0(itsCoords[0]),
  coord1(itsCoords[0]),
  coord2(itsCoords[0]),
  coord3(itsCoords[0])
{
DOTRACE("Fish::Fish");
  itsFishParts = new FishPart[4];
  itsEndPts = new EndPt[4];

  read_splinefile(splinefile);
  read_coordfile(coordfile, index);

}


Fish::~Fish () {
DOTRACE("Fish::~Fish");
  delete [] itsFishParts;
  delete [] itsEndPts;
}

///////////////////////////////////////////////////////////////////////
//
// Properties
//
///////////////////////////////////////////////////////////////////////

const vector<Fish::PInfo>& Fish::getPropertyInfos() {
DOTRACE("Fish::getPropertyInfos");

  static vector<PInfo> p;

  typedef Fish F;

  if (p.size() == 0) {
	 p.push_back(PInfo("currentPart", &F::currentPart, 0, 3, 1, true));
	 p.push_back(PInfo("currentEndPt", &F::currentEndPt, 0, 3, 1));

	 p.push_back(PInfo("coord0", &F::coord0, -2.0, 2.0, 0.1));
	 p.push_back(PInfo("coord1", &F::coord1, -2.0, 2.0, 0.1));
	 p.push_back(PInfo("coord2", &F::coord2, -2.0, 2.0, 0.1));
	 p.push_back(PInfo("coord3", &F::coord3, -2.0, 2.0, 0.1));
  }
  return p;
}
void Fish::read_splinefile(const char* splinefile) {
DOTRACE("Fish::read_splinefile");
  int   i, j, k, rval_int, splnb, endptnb;
  char *nb_ptr, line[256], *rval_ptr;
  FILE  *fp;

  // reads in the spline knots and coefficient
  fp = fopen(splinefile, "r");

  for(i = 0; i < 4; ++i) {
    // spline number and order
    rval_ptr = fgets(line, 256, fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }
    rval_int = sscanf(line, "%*s %i %*s %i", &splnb, &(itsFishParts[i].itsOrder));
    if ( rval_int != 2 ){
      throw ErrorWithMsg(string("error scanning the spline line: ")
										  /* + rval_int */);
    }

    // number of knots
    rval_ptr = fgets(line, 256, fp);
    if ( rval_ptr == NULL ){
      throw ErrorWithMsg(string("error reading line: ") + line);
    }

	 int nknots;

    rval_int = sscanf(line, "%*s %i", &nknots);
    if ( rval_int != 1 ){
      throw ErrorWithMsg(string("error scanning the spline line: ")
										  /* + rval_int */);
    }

    // allocates space and reads in the successive knots
	 itsFishParts[i].itsKnots.resize(nknots);
    rval_ptr = fgets(line, 256, fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }

    nb_ptr = strtok(line, " ");
    sscanf(nb_ptr, "%f", &(itsFishParts[i].itsKnots[0]));
    for (j = 1; j < itsFishParts[i].itsKnots.size();j++){
      nb_ptr = strtok(NULL, " ");
      sscanf(nb_ptr, "%f", &(itsFishParts[i].itsKnots[j]));
    }

    // number of coefficients
    rval_ptr = fgets(line, 256, fp);
    if ( rval_ptr == NULL ){
      throw ErrorWithMsg(string("error reading line: ") + line);
    }

	 int ncoefs;

    rval_int = sscanf(line, "%*s %i", &ncoefs);
    if ( rval_int != 1 ){
      throw ErrorWithMsg(string("error scanning the spline line: ")
										  /* + rval_int */);
    }

    // allocates space and reads in the successive coefficients
    for (j = 0; j < 2; ++j){

  		itsFishParts[i].itsCoefs[j].resize(ncoefs);

      rval_ptr = fgets(line, 256, fp);
      if ( rval_ptr == NULL ){
		  throw ErrorWithMsg(string("error reading line: ") + line);
      }

      nb_ptr = strtok(line, " ");
		sscanf(nb_ptr, "%f", &(itsFishParts[i].itsCoefs[j][0]));
      for (k = 1; k < ncoefs; ++k) {
        nb_ptr = strtok(NULL, " ");
  		  sscanf(nb_ptr, "%f", &(itsFishParts[i].itsCoefs[j][k]));
      }
    }
  }


  for(i = 0; i < 4; ++i) {
    // endpt number, associated part and breakpoint
    rval_ptr = fgets(line, 256, fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }
    rval_int = sscanf(line, "%*s %i %*s %i %*s %i",
		      &endptnb, &(itsEndPts[i].itsPart), &(itsEndPts[i].itsBkpt));
    if ( rval_int != 3 ){
      throw ErrorWithMsg(
			string("error scanning the endpoint "
					 "definition line: ") /* + rval_int */);
    }

    // skip the next line
    rval_ptr = fgets(line, 256, fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }

    // x coordinates
    rval_ptr = fgets(line, 256, fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }
    nb_ptr = strtok(line, " ");
    sscanf(nb_ptr, "%f", &(itsEndPts[i].itsX[0]));
    nb_ptr = strtok(NULL, " ");
    sscanf(nb_ptr, "%f", &(itsEndPts[i].itsX[1]));


    // y coordinates
    rval_ptr = fgets(line, 256, fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }
    nb_ptr = strtok(line, " ");
    sscanf(nb_ptr, "%f", &(itsEndPts[i].itsY[0]));
    nb_ptr = strtok(NULL, " ");
    sscanf(nb_ptr, "%f", &(itsEndPts[i].itsY[1]));

  }
}

void Fish::read_coordfile(const char* coordfile, int index) {
DOTRACE("Fish::read_coordfile");
  int class1;
  char *next_ptr, line[256];

  FILE *fp = fopen(coordfile, "r");

  // Read 'index' number of lines, the last one (the index'th one) is
  // the keeper.
  for (int j = 0; j < index; ++j) {
    fgets(line, 256, fp);
  }

  next_ptr = strtok(line, " ");

  sscanf(next_ptr, "%d", &class1);
  DebugPrintNL(class1);

  for (int i = 0; i < 4; ++i){
    next_ptr = strtok(NULL, " ");
    sscanf(next_ptr, "%lf", itsCoords+i);
	 DebugEval(itsCoords[i]);
  }
  DebugPrintNL("");
}

void Fish::grRender() const {
DOTRACE("Fish::grRender");
  int j, k;

  // Create and configure the NURBS object
  GLUnurbsObj* theNurb = gluNewNurbsRenderer();
  if (theNurb == 0) {
	 throw ErrorWithMsg("Fish::grRender: couldn't allocate GLUnurbsObj");
  }
  gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
  gluNurbsProperty(theNurb, GLU_U_STEP, 200);
  gluNurbsProperty(theNurb, GLU_V_STEP, 200);

  // Loop over fish parts
  for (int i = 0; i < 4; ++i) {
	 int totcoefs = 3*(itsFishParts[i].itsCoefs[0].size());
	 vector<GLfloat> coefs(totcoefs);

	 for (j = 0; j < itsFishParts[i].itsCoefs[0].size(); ++j) {
      for (k = 0; k < 2; ++k){
  		  coefs[3*j+k] = (GLfloat) itsFishParts[i].itsCoefs[k][j];
      }
      coefs[3*j+2] = (GLfloat) 0.0;
    }

	 // Fix up the coefficients at the end points
    for (j = 0; j < 4; ++j) {
      if ( itsEndPts[j].itsPart == (i+1) ) {
		  int bkpt = itsEndPts[j].itsBkpt - 1; // indexing in the coefs array starts at 0
        float alpha = 0.5*(1-itsCoords[j]);
		  float beta  = 0.5*(1+itsCoords[j]);
		  GLfloat x = alpha*itsEndPts[j].itsX[0] + beta*itsEndPts[j].itsX[1];
		  GLfloat y = alpha*itsEndPts[j].itsY[0] + beta*itsEndPts[j].itsY[1];
		  coefs[3*bkpt]   = x;
		  coefs[3*bkpt+1] = y;
      }
    }

	 // Render the curve
    gluBeginCurve(theNurb);
	 {
		gluNurbsCurve(theNurb,
						  itsFishParts[i].itsKnots.size(),
						  &(itsFishParts[i].itsKnots[0]),
						  3, &coefs[0],
						  itsFishParts[i].itsOrder, GL_MAP1_VERTEX_3);
	 }
    gluEndCurve(theNurb);

  } // end loop over fish parts

  // Must free theNurb here
  gluDeleteNurbsRenderer(theNurb);
}

static const char vcid_fish_cc[] = "$Header$";
#endif // !FISH_CC_DEFINED
