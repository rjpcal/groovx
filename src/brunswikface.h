///////////////////////////////////////////////////////////////////////
// brunswikface.h
// Rob Peters
// created: Thu Jan 28 13:04:37 1999
// written: Tue Mar 16 19:46:57 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef BRUNSWIKFACE_H_DEFINED
#define BRUNSWIKFACE_H_DEFINED

#include "facerep.h"

class BrunswikFace : private FaceRep {
public:
  typedef FaceRep::Pointf Pointf;
  BrunswickFace(float nl=0.4, float ed=0.4, float eh=0.6, float mh=-0.8)
    : itsLeftEye(-ed/2.0, eh),
      itsRightEye(ed/2.0, eh),
      itsLeftPupil(-ed/2.0, eh),
      itsRightPupil(ed/2.0, eh),
      itsMouthLeft(-0.2, mh),
      itsMouthRight(0.2, mh),
      itsNoseBottom(0.0, -nl/2.0),
      itsNoseTop(0.0, -nl/2.0),
      itsLeft(),
      itsRight(),
      itsBottom(),
      itsTop() {}

  virtual ~BrunswickFace() {}

  setNoseLength(float nl) { itsNoseBottom.y = -nl/2.0; itsNoseTop.y = nl/2.0; }
  setEyeDist(float ed) { 
    itsLeftEye.x = itsLeftPupil.x = -ed/2.0; 
    itsRightEye.x = itsRightPupil.x = ed/2.0;   
  }
  setEyeHeight(float eh) { itsLeftEye.y = itsRightEye.y = eh; }
  setMouthHeight(float mh) { itsMouthLeft.y = itsMouthRight.y = mh; }
};

static const char vcid_brunswikface_h[] = "$Header$";
#endif // !BRUNSWIKFACE_H_DEFINED
