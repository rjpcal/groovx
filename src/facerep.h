///////////////////////////////////////////////////////////////////////
// facerep.h
// Rob Peters
// created: Thu Jan 28 12:36:12 1999
// written: Tue Mar 16 19:44:34 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef FACEREP_H_DEFINED
#define FACEREP_H_DEFINED

#ifdef POINT_H_INCLUDED
#include "point.h"
#endif

class FaceRep {
public:
  typedef Point<float> Pointf;
  FaceRep() {}
  virtual ~FaceRep() {}
protected:
  Pointf itsLeftEye;
  Pointf itsRightEye;
  Pointf itsLeftPupil;
  Pointf itsRightPupil;
  Pointf itsMouthLeft;
  Pointf itsMouthRight;
  Pointf itsNoseBottom;
  Pointf itsNoseTop;
  Pointf itsLeft;
  Pointf itsRight;
  Pointf itsBottom;
  Pointf itsTop;
};

static const char vcid_facerep_h[] = "$Id$";
#endif // !FACEREP_H_DEFINED
