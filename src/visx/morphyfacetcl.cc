///////////////////////////////////////////////////////////////////////
//
// morphyfacetcl.cc
// Rob Peters 
// created: Wed Sep  8 15:42:36 1999
// written: Thu Sep  9 18:26:11 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include <tcl.h>
#include <fstream.h>
#include <strstream.h>
#include <cstring>
#include <cctype>
#include <vector>

#include "iomgr.h"
#include "objlist.h"
#include "listitempkg.h"
#include "morphyface.h"
#include "tclcmd.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace MorphyFaceTcl {
  class LoadFacesCmd;
  class MorphyFacePkg;
}

///////////////////////////////////////////////////////////////////////
//
// MorphyFacePkg class definition
//
///////////////////////////////////////////////////////////////////////

/*
  IoWrapper<double> its\(.*\);
	 declareCAttrib("\1", &MorphyFace::get\1, &MorphyFace::set\1);
*/

class MorphyFaceTcl::MorphyFacePkg : public ListItemPkg<MorphyFace, ObjList> {
public:
  MorphyFacePkg(Tcl_Interp* interp) :
	 ListItemPkg<MorphyFace, ObjList>(interp, ObjList::theObjList(), "MorphyFace", "2.5")
  {
	 declareCAttrib("faceWidth", &MorphyFace::getFaceWidth, &MorphyFace::setFaceWidth);
	 declareCAttrib("topWidth", &MorphyFace::getTopWidth, &MorphyFace::setTopWidth);
	 declareCAttrib("bottomWidth", &MorphyFace::getBottomWidth, &MorphyFace::setBottomWidth);
	 declareCAttrib("topHeight", &MorphyFace::getTopHeight, &MorphyFace::setTopHeight);
	 declareCAttrib("bottomHeight", &MorphyFace::getBottomHeight, &MorphyFace::setBottomHeight);

	 declareCAttrib("eyeYpos", &MorphyFace::getEyeYpos, &MorphyFace::setEyeYpos);
	 declareCAttrib("eyeDist", &MorphyFace::getEyeDist, &MorphyFace::setEyeDist);
	 declareCAttrib("eyeHeight", &MorphyFace::getEyeHeight, &MorphyFace::setEyeHeight);
	 declareCAttrib("eyeAspectRatio", &MorphyFace::getEyeAspectRatio, &MorphyFace::setEyeAspectRatio);

	 declareCAttrib("pupilXpos", &MorphyFace::getPupilXpos, &MorphyFace::setPupilXpos);
	 declareCAttrib("pupilYpos", &MorphyFace::getPupilYpos, &MorphyFace::setPupilYpos);
	 declareCAttrib("pupilSize", &MorphyFace::getPupilSize, &MorphyFace::setPupilSize);
	 declareCAttrib("pupilDilation", &MorphyFace::getPupilDilation, &MorphyFace::setPupilDilation);

	 declareCAttrib("eyebrowXpos", &MorphyFace::getEyebrowXpos, &MorphyFace::setEyebrowXpos);
	 declareCAttrib("eyebrowYpos", &MorphyFace::getEyebrowYpos, &MorphyFace::setEyebrowYpos);
	 declareCAttrib("eyebrowCurvature", &MorphyFace::getEyebrowCurvature, &MorphyFace::setEyebrowCurvature);
	 declareCAttrib("eyebrowAngle", &MorphyFace::getEyebrowAngle, &MorphyFace::setEyebrowAngle);
	 declareCAttrib("eyebrowThickness", &MorphyFace::getEyebrowThickness, &MorphyFace::setEyebrowThickness);

	 declareCAttrib("noseXpos", &MorphyFace::getNoseXpos, &MorphyFace::setNoseXpos);
	 declareCAttrib("noseYpos", &MorphyFace::getNoseYpos, &MorphyFace::setNoseYpos);
	 declareCAttrib("noseLen", &MorphyFace::getNoseLen, &MorphyFace::setNoseLen);
	 declareCAttrib("noseWidth", &MorphyFace::getNoseWidth, &MorphyFace::setNoseWidth);

	 declareCAttrib("mouthXpos", &MorphyFace::getMouthXpos, &MorphyFace::setMouthXpos);
	 declareCAttrib("mouthYpos", &MorphyFace::getMouthYpos, &MorphyFace::setMouthYpos);
	 declareCAttrib("mouthWidth", &MorphyFace::getMouthWidth, &MorphyFace::setMouthWidth);
	 declareCAttrib("mouthCurvature", &MorphyFace::getMouthCurvature, &MorphyFace::setMouthCurvature);

  }
};
  
extern "C" Tcl_PackageInitProc Morphyface_Init;

int Morphyface_Init(Tcl_Interp* interp) {
DOTRACE("Morphyface_Init");

  new MorphyFaceTcl::MorphyFacePkg(interp);

  FactoryRegistrar<IO, MorphyFace> registrar(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_morphyfacetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
