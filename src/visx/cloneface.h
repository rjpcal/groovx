///////////////////////////////////////////////////////////////////////
//
// cloneface.h
// Rob Peters
// created: Thu Apr 29 09:19:25 1999
// written: Thu Jun  3 11:01:23 1999
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

  virtual const float* getCtrlPnts() const;
  // Returns an array of Bezier control points for face outline

  virtual float getEyeAspect() const;
  // Returns the aspect ratio of eye outline.
  
  virtual float getVertOffset() const;
  // Returns the amount of vertical offset applied to all features.

private:
  float itsCtrlPnts[24];		  // Bezier control points for face outline
  float itsEyeAspect;			  // control aspect ratio of eye outline
  float itsVertOffset;			  // amount of vertical offset of all features
};

static const char vcid_cloneface_h[] = "$Header$";
#endif // !CLONEFACE_H_DEFINED
