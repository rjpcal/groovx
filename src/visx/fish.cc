///////////////////////////////////////////////////////////////////////
//
// fish.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 11:44:57 1999
// written: Tue Nov 30 16:57:35 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISH_CC_DEFINED
#define FISH_CC_DEFINED

#include "fish.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <fstream.h>
#include <strstream.h>

#include "error.h"
#include "reader.h"
#include "rect.h"
#include "writer.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string ioTag = "Fish";

  int dummy=0; // We need a dummy int to attach various CPtrProperty's
}

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
  coord0(itsCoords[0]),
  coord1(itsCoords[1]),
  coord2(itsCoords[2]),
  coord3(itsCoords[3]),
  currentPart(0),
  currentEndPt(0),
  endPt_Part(dummy),
  endPt_Bkpt(dummy)
{
DOTRACE("Fish::Fish");
  itsFishParts = new FishPart[4];
  itsEndPts = new EndPt[4];

  if (splinefile != 0 && coordfile != 0) {
	 readSplineFile(splinefile);
	 readCoordFile(coordfile, index);
  }
  else {
	 restoreToDefault();
  }

//    currentPart.attach(this);
//    currentEndPt.attach(this);

//    currentPart.sendStateChangeMsg();
//    currentEndPt.sendStateChangeMsg();
  sendStateChangeMsg();
}

void Fish::restoreToDefault() {
DOTRACE("Fish::restoreToDefault");
  itsFishParts[3].itsOrder = itsFishParts[2].itsOrder = itsFishParts[1].itsOrder = itsFishParts[0].itsOrder = 4;

  static GLfloat default_knots[] = {0.0, 0.0, 0.0, 0.0, 0.1667, 0.3333, 0.5000, 0.6667, 0.8333, 1.0, 1.0, 1.0, 1.0};
  itsFishParts[0].itsKnots.assign(default_knots, default_knots+13);
  itsFishParts[1].itsKnots.assign(default_knots, default_knots+13);
  itsFishParts[2].itsKnots.assign(default_knots, default_knots+13);
  itsFishParts[3].itsKnots.assign(default_knots, default_knots+13);

  // coefficients 0
  static GLfloat coefs0_0[] = {-0.2856, -0.2140, -0.2176, -0.0735, 0.0168, 0.1101, 0.3049, 0.0953, 0.1597};
  static GLfloat coefs0_1[] = {0.2915, 0.2866, 0.2863, 0.4670, 0.3913, 0.3071, 0.1048, 0.1800, 0.1538};
  itsFishParts[0].itsCoefs[0].assign(coefs0_0, coefs0_0+9);
  itsFishParts[0].itsCoefs[1].assign(coefs0_1, coefs0_1+9);

  // coefficients 1
  static GLfloat coefs1_0[] = {0.1597, 0.2992, 0.2864, 0.7837, 0.4247, 0.6362, 0.3379, 0.3137, 0.1573};
  static GLfloat coefs1_1[] = {0.1538, 0.1016, 0.1044, 0.1590, 0.0155, -0.3203, -0.0706, 0.0049, -0.0401};
  itsFishParts[1].itsCoefs[0].assign(coefs1_0, coefs1_0+9);
  itsFishParts[1].itsCoefs[1].assign(coefs1_1, coefs1_1+9);

  // coefficients 2
  static GLfloat coefs2_0[] = {0.1573, 0.2494, 0.1822, -0.0208, -0.0632, -0.1749, -0.1260, -0.3242, -0.2844};
  static GLfloat coefs2_1[] = {-0.0401, -0.0294, -0.0877, -0.3236, -0.3412, -0.1120, -0.4172, -0.1818, -0.1840};
  itsFishParts[2].itsCoefs[0].assign(coefs2_0, coefs2_0+9);
  itsFishParts[2].itsCoefs[1].assign(coefs2_1, coefs2_1+9);

  // coefficients 3
  static GLfloat coefs3_0[] = {-0.2844, -0.3492, -0.4554, -0.6135, -0.7018, -0.5693, -0.4507, -0.3393, -0.2856};
  static GLfloat coefs3_1[] = {-0.1840, -0.1834, -0.1489, -0.0410, 0.0346, 0.1147, 0.2227, 0.2737, 0.2915};
  itsFishParts[3].itsCoefs[0].assign(coefs3_0, coefs3_0+9);
  itsFishParts[3].itsCoefs[1].assign(coefs3_1, coefs3_1+9);

  itsEndPts[0].itsPart = 1;
  itsEndPts[0].itsBkpt = 6;
  itsEndPts[0].itsX[0] = 0.2380;
  itsEndPts[0].itsX[1] = -0.0236;
  itsEndPts[0].itsY[0] = 0.3416;
  itsEndPts[0].itsY[1] = 0.2711;

  itsEndPts[1].itsPart = 2;
  itsEndPts[1].itsBkpt = 5;
  itsEndPts[1].itsX[0] = 0.6514;
  itsEndPts[1].itsX[1] = 0.2433;
  itsEndPts[1].itsY[0] = -0.0305;
  itsEndPts[1].itsY[1] = 0.0523;

  itsEndPts[2].itsPart = 3;
  itsEndPts[2].itsBkpt = 6;
  itsEndPts[2].itsX[0] = -0.2121;
  itsEndPts[2].itsX[1] = -0.1192;
  itsEndPts[2].itsY[0] = 0.0083;
  itsEndPts[2].itsY[1] = -0.2925;

  itsEndPts[3].itsPart = 4;
  itsEndPts[3].itsBkpt = 5;
  itsEndPts[3].itsX[0] = -0.7015;
  itsEndPts[3].itsX[1] = -0.7022;
  itsEndPts[3].itsY[0] = 0.1584;
  itsEndPts[3].itsY[1] = -0.1054;

  itsCoords[0] = itsCoords[1] = itsCoords[2] = itsCoords[3] = 0.0;
}

Fish::~Fish () {
DOTRACE("Fish::~Fish");
  delete [] itsFishParts;
  delete [] itsEndPts;
}

void Fish::serialize(ostream& os, IOFlag flag) const {
DOTRACE("Fish::serialize");

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  vector<const IO *> ioList;
  makeIoList(ioList);
  for (vector<const IO *>::const_iterator ii = ioList.begin();
		 ii != ioList.end(); ++ii) {
	 (*ii)->serialize(os, flag);
  }

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { GrObj::serialize(os, flag | TYPENAME); }  
}

void Fish::deserialize(istream& is, IOFlag flag) {
DOTRACE("Fish::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  vector<IO *> ioList;
  makeIoList(ioList);
  for (vector<IO *>::iterator ii = ioList.begin(); ii != ioList.end(); ii++) {
	 (*ii)->deserialize(is, flag);
  }

  try {
	 if (is.fail()) throw InputError(ioTag);
  }
  catch (IoError&) { 
	 throw;
  }

  if (flag & BASES) { GrObj::deserialize(is, flag | TYPENAME); }

  sendStateChangeMsg();
}

int Fish::charCount() const {
DOTRACE("Fish::charCount");
  int result = ioTag.length() + 1;

  vector<const IO*> ioList;
  makeIoList(ioList);

  for (size_t i = 0; i < ioList.size(); ++i) {
	 result += ioList[i]->charCount() + 1; 
  }

  result += 5;						  // fudge factor

  result += GrObj::charCount();

  return result;
}

void Fish::readFrom(Reader* reader) {
DOTRACE("Fish::readFrom");
  const vector<PInfo>& infos = getPropertyInfos();
  for (size_t i = 0; i < infos.size(); ++i) {
	 reader->readValueObj(infos[i].name, const_cast<Value&>(get(infos[i].property)));
  }

  GrObj::readFrom(reader);
}

void Fish::writeTo(Writer* writer) const {
DOTRACE("Fish::writeTo");
  const vector<PInfo>& infos = getPropertyInfos();
  for (size_t i = 0; i < infos.size(); ++i) {
	 writer->writeValueObj(infos[i].name, get(infos[i].property));
  }

  GrObj::writeTo(writer);
}

void Fish::receiveStateChangeMsg(const Observable* obj) {
DOTRACE("Fish::receiveStateChangeMsg");
//    if (obj == &currentPart) {
//    } 
//    else if (obj == &currentEndPt) {
//  	 endPt_Part.reseat(itsEndPts[currentEndPt()].itsPart);
//  	 endPt_Bkpt.reseat(itsEndPts[currentEndPt()].itsBkpt);
//    }
//    else {
//  	 GrObj::receiveStateChangeMsg(obj);
//    }
 
  endPt_Part.reseat(itsEndPts[currentEndPt()].itsPart);
  endPt_Bkpt.reseat(itsEndPts[currentEndPt()].itsBkpt);
   
  GrObj::receiveStateChangeMsg(obj);
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
	 p.push_back(PInfo("category", &F::category, 0, 10, 1, true));
	 p.push_back(PInfo("coord0", &F::coord0, -2.0, 2.0, 0.1));
	 p.push_back(PInfo("coord1", &F::coord1, -2.0, 2.0, 0.1));
	 p.push_back(PInfo("coord2", &F::coord2, -2.0, 2.0, 0.1));
	 p.push_back(PInfo("coord3", &F::coord3, -2.0, 2.0, 0.1));

	 p.push_back(PInfo("currentPart", &F::currentPart, 0, 3, 1, true));

	 p.push_back(PInfo("currentEndPt", &F::currentEndPt, 0, 3, 1, true));
	 p.push_back(PInfo("endPt_Part", &F::endPt_Part, 1, 4, 1));
	 p.push_back(PInfo("endPt_Bkpt", &F::endPt_Bkpt, 1, 10, 1));
  }
  return p;
}

void Fish::readSplineFile(const char* splinefile) {
DOTRACE("Fish::readSplineFile");
  size_t i, j; 
  int k, splnb, endptnb;
  string dummy;

  // reads in the spline knots and coefficient
  ifstream ifs(splinefile);
  if (ifs.fail()) {
	 throw ErrorWithMsg(string("error opening file '") + splinefile + "'");
  }

  for(i = 0; i < 4; ++i) {
    // spline number and order
	 ifs >> dummy >> splnb >> dummy >> itsFishParts[i].itsOrder;

    // number of knots
	 int nknots;
	 ifs >> dummy >> nknots;

    // allocates space and reads in the successive knots
	 itsFishParts[i].itsKnots.resize(nknots);
	 for (j = 0; j < itsFishParts[i].itsKnots.size(); ++j) {
		ifs >> itsFishParts[i].itsKnots[j];
	 }

    // number of coefficients
	 int ncoefs;
	 ifs >> dummy >> ncoefs;

    // allocates space and reads in the successive coefficients
    for (j = 0; j < 2; ++j) {
  		itsFishParts[i].itsCoefs[j].resize(ncoefs);

		for (k = 0; k < ncoefs; ++k) {
		  ifs >> itsFishParts[i].itsCoefs[j][k];
		}
    }

	 if (ifs.fail()) {
		throw ErrorWithMsg(string("error reading file '") + splinefile + "'");
	 }
  }


  for(i = 0; i < 4; ++i) {
    // endpt number, associated part and breakpoint
	 ifs >> dummy >> endptnb
		  >> dummy >> itsEndPts[i].itsPart
		  >> dummy >> itsEndPts[i].itsBkpt;

    // skip the next line
	 ifs >> dummy >> dummy;

    // x&y coordinates
	 ifs >> itsEndPts[i].itsX[0] >> itsEndPts[i].itsX[1]
		  >> itsEndPts[i].itsY[0] >> itsEndPts[i].itsY[1];

	 if (ifs.fail()) {
		throw ErrorWithMsg(string("error reading file '") + splinefile + "'");
	 }
  }
}

void Fish::readCoordFile(const char* coordfile, int index) {
DOTRACE("Fish::readCoordFile");
  string dummy;

  ifstream ifs(coordfile);
  if (ifs.fail()) {
	 throw ErrorWithMsg(string("error opening file '") + coordfile + "'");
  }

  // Skip (index-1) lines
  for (int j = 0; j < index-1; ++j) {
	 getline(ifs, dummy, '\n');
  }

  ifs >> category();

  for (int i = 0; i < 4; ++i) {
	 ifs >> itsCoords[i];
  }

  if (ifs.fail()) {
	 throw ErrorWithMsg(string("error reading file '") + coordfile + "'");
  }
}

bool Fish::grGetBoundingBox(Rect<double>& bbox,
									 int& border_pixels) const {
DOTRACE("Fish::grGetBoundingBox");
  bbox.left() = -0.75;
  bbox.right() = 0.75;
  bbox.bottom() = -0.5;
  bbox.top() = 0.5;
  border_pixels = 4;
  
  return true;
}

void Fish::grRender() const {
DOTRACE("Fish::grRender");
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

	 {
		for (size_t j = 0; j < itsFishParts[i].itsCoefs[0].size(); ++j) {
		  for (int k = 0; k < 2; ++k) {
			 coefs[3*j+k] = (GLfloat) itsFishParts[i].itsCoefs[k][j];
		  }
		  coefs[3*j+2] = (GLfloat) 0.0;
		}
	 }

	 // Fix up the coefficients at the end points
    for (int j = 0; j < 4; ++j) {
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

void Fish::makeIoList(vector<IO *>& vec) {
DOTRACE("Fish::makeIoList");
  makeIoList(reinterpret_cast<vector<const IO *> &>(vec)); 
}

void Fish::makeIoList(vector<const IO *>& vec) const {
DOTRACE("Fish::makeIoList const");
  vec.clear();

  vec.push_back(&category);
  vec.push_back(&coord0);
  vec.push_back(&coord1);
  vec.push_back(&coord2);
  vec.push_back(&coord3);
}


static const char vcid_fish_cc[] = "$Header$";
#endif // !FISH_CC_DEFINED
