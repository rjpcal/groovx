///////////////////////////////////////////////////////////////////////
//
// cloneface.h
// Rob Peters
// created: Thu Apr 29 09:19:25 1999
// written: Thu Oct 21 18:09:27 1999
// $Id$
//
// CloneFace is a modified version of Face that allows additional
// parameters to be modified, such as the Bezier control points for
// the face outline, and the shape of the eye outline. The default
// behavior of a CloneFace will be identical to that of a Face.
//
///////////////////////////////////////////////////////////////////////

#ifndef CLONEFACE_H_DEFINED
#define CLONEFACE_H_DEFINED

#include "face.h"

///////////////////////////////////////////////////////////////////////
//
// CloneFace class definition
//
///////////////////////////////////////////////////////////////////////

class CloneFace : public Face {
public:
  CloneFace ();
  CloneFace (istream &is, IOFlag flag);
  virtual ~CloneFace();
  
  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  virtual const double* getCtrlPnts() const;
  // Returns an array of Bezier control points for face outline

  virtual double getEyeAspect() const;
  // Returns the aspect ratio of eye outline.
  
  virtual double getVertOffset() const;
  // Returns the amount of vertical offset applied to all features.

private:
  double itsCtrlPnts[24];		  // Bezier control points for face outline
  double itsEyeAspect;			  // control aspect ratio of eye outline
  double itsVertOffset;			  // amount of vertical offset of all features
};

static const char vcid_cloneface_h[] = "$Header$";
#endif // !CLONEFACE_H_DEFINED
