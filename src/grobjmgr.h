///////////////////////////////////////////////////////////////////////
// grobjmgr.h
// Rob Peters
// created: Mon Mar  8 00:33:26 1999
// written: Fri Mar 12 12:56:49 1999
static const char vcid_grobjmgr_h[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef GROBJMGR_H_DEFINED
#define GROBJMGR_H_DEFINED

class istream;
class GrObj;

namespace GrobjMgr {
  GrObj* newGrobj(istream &is);
  GrObj* newGrobj(const char* type, istream &is);
}

#endif // !GROBJMGR_H_DEFINED
