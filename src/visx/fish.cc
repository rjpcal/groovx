///////////////////////////////////////////////////////////////////////
//
// fish.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 11:44:57 1999
// written: Thu Sep 30 12:30:33 1999
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

#include "error.h"

Fish::Fish(const char* splinefile, const char* coordfile, int index) {
  read_splinefile(splinefile);
  read_coordfile(coordfile, index);
}


Fish::~Fish () {

}

void Fish::read_splinefile(const char* splinefile) {
  int   i, j, k, rval_int, splnb, endptnb;
  char *nb_ptr, line[256], *rval_ptr;
  FILE  *fp;

  //reads in the spline knots and coefficient
  fp = fopen(splinefile,"r");

  for(i = 0; i < 4; ++i) {
    //spline number and order
    rval_ptr = fgets(line,256,fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }
    rval_int = sscanf(line,"%*s %i %*s %i",&splnb,&(fish[i].order)); 
    if ( rval_int != 2 ){
      throw ErrorWithMsg(string("error scanning the spline line: ")
										  /* + rval_int */);
    }

    //number of knots
    rval_ptr = fgets(line,256,fp);
    if ( rval_ptr == NULL ){
      throw ErrorWithMsg(string("error reading line: ") + line);
    }
    rval_int = sscanf(line,"%*s %i",&fish[i].nknots); 
    if ( rval_int != 1 ){
      throw ErrorWithMsg(string("error scanning the spline line: ")
										  /* + rval_int */);
    } 

    //allocates space and reads in the successive knots
    fish[i].knots = (float *) calloc(fish[i].nknots,sizeof(float));
    rval_ptr = fgets(line,256,fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }

    nb_ptr = strtok(line," ");
    sscanf(nb_ptr,"%f",&(fish[i].knots[0]));
    for (j=1;j<fish[i].nknots;j++){
      nb_ptr = strtok(NULL," ");
      sscanf(nb_ptr,"%f",&(fish[i].knots[j]));
    }

    //number of coefficients
    rval_ptr = fgets(line,256,fp);
    if ( rval_ptr == NULL ){
      throw ErrorWithMsg(string("error reading line: ") + line);
    }
    rval_int = sscanf(line,"%*s %i",&(fish[i].ncoefs)); 
    if ( rval_int != 1 ){
      throw ErrorWithMsg(string("error scanning the spline line: ")
										  /* + rval_int */);
    }
  
    //allocates space and reads in the successive coefficients
    for (j=0;j<2;j++){
      fish[i].coefs[j] = (float *) calloc(fish[i].ncoefs,sizeof(float));  

      rval_ptr = fgets(line,256,fp);
      if ( rval_ptr == NULL ){
		  throw ErrorWithMsg(string("error reading line: ") + line);
      }

      nb_ptr = strtok(line," ");
      sscanf(nb_ptr,"%f",&(fish[i].coefs[j][0]));
      for (k=1;k<fish[i].ncoefs;k++){
        nb_ptr = strtok(NULL," ");
	sscanf(nb_ptr,"%f",&(fish[i].coefs[j][k]));
      }
    }
  }


  for(i=0;i<4;i++){
    //endpt number, associated part and breakpoint
    rval_ptr = fgets(line,256,fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }
    rval_int = sscanf(line,"%*s %i %*s %i %*s %i",
		      &endptnb,&(endpts[i].part),&(endpts[i].bkpt)); 
    if ( rval_int != 3 ){
      throw ErrorWithMsg(
			string("error scanning the endpoint "
					 "definition line: ") /* + rval_int */);
    }

    //skip the next line
    rval_ptr = fgets(line,256,fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }

    //x coordinates
    rval_ptr = fgets(line,256,fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }
    nb_ptr = strtok(line," ");
    sscanf(nb_ptr,"%f",&(endpts[i].x[0]));
    nb_ptr = strtok(NULL," ");
    sscanf(nb_ptr,"%f",&(endpts[i].x[1]));


    //y coordinates
    rval_ptr = fgets(line,256,fp);
    if ( rval_ptr == NULL ){
		throw ErrorWithMsg(string("error reading line: ") + line);
    }
    nb_ptr = strtok(line," ");
    sscanf(nb_ptr,"%f",&(endpts[i].y[0]));
    nb_ptr = strtok(NULL," ");
    sscanf(nb_ptr,"%f",&(endpts[i].y[1]));

  }
}

void Fish::read_coordfile(const char* coordfile, int index) {
  int i, class1;
  char *next_ptr, line[256];
  FILE *fp;

  fp = fopen(coordfile,"r");

  for (i=0;i<index;i++)
    fgets(line,256,fp);
  
  next_ptr = strtok(line," ");
  sscanf(next_ptr,"%f",&class1);
  for (i=0;i<4;i++){
    next_ptr = strtok(NULL," ");
    sscanf(next_ptr,"%f",fishcoord+i);
  }

}

void Fish::grRender() const {
  int j, k;

  // Loop over fish parts
  for (int i = 0; i < 4; ++i) {
    GLint order = (GLint) fish[i].order;
    GLint nknots = (GLint) fish[i].nknots;

	 vector<GLfloat> knots(nknots);

    for (j = 0; j < nknots; ++j) {
      knots[j] = (GLfloat) fish[i].knots[j];
	 }

    int totcoefs = 3*(fish[i].ncoefs);
	 vector<GLfloat> coefs(totcoefs);

    for (j = 0; j < (fish+i)->ncoefs; ++j) {
      for (k = 0; k < 2; ++k){
		  coefs[3*j+k] = (GLfloat) fish[i].coefs[k][j];
      }
      coefs[3*j+2] = (GLfloat) 0.0;
    }

	 // Fix up the coefficients at the end points
    for (j = 0; j < 4; ++j) {
      if ( endpts[j].part == (i+1) ) {
		  int bkpt = endpts[j].bkpt - 1; //indexing in the coefs array starts at 0
        float alpha = 0.5*(1-fishcoord[j]);
		  float beta  = 0.5*(1+fishcoord[j]);
		  GLfloat x = alpha*endpts[j].x[0] + beta*endpts[j].x[1];
		  GLfloat y = alpha*endpts[j].y[0] + beta*endpts[j].y[1];
		  coefs[3*bkpt]   = x;
		  coefs[3*bkpt+1] = y;
      }
    }

	 // Create and configure the NURBS object
    GLUnurbsObj* theNurb = gluNewNurbsRenderer();
    gluNurbsProperty(theNurb,GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
    gluNurbsProperty(theNurb,GLU_U_STEP,200);
    gluNurbsProperty(theNurb,GLU_V_STEP,200);

	 // Render the curve
    gluBeginCurve(theNurb);
	 {
		gluNurbsCurve(theNurb,
						  nknots, &knots[0],
						  3, &coefs[0],
						  order, GL_MAP1_VERTEX_3);
	 }
    gluEndCurve(theNurb);

	 // XXX Must free theNurb here

  } // end loop over fish parts
}

static const char vcid_fish_cc[] = "$Header$";
#endif // !FISH_CC_DEFINED
