///////////////////////////////////////////////////////////////////////
// facerep.h
// Rob Peters
// created: Thu Jan 28 12:36:12 1999
// written: Mon Mar  8 17:07:30 1999
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

#endif // !FACEREP_H_DEFINED
