///////////////////////////////////////////////////////////////////////
// grobjmgr.h
// Rob Peters
// created: Mon Mar  8 00:33:26 1999
// written: Sun Mar 14 18:14:01 1999
///////////////////////////////////////////////////////////////////////

#ifndef GROBJMGR_H_DEFINED
#define GROBJMGR_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

class istream;
class GrObj;

namespace GrobjMgr {
  GrObj* newGrobj(istream &is, IOFlag flag);
  GrObj* newGrobj(const char* type, istream &is, IOFlag flag);
}

static const char vcid_grobjmgr_h[] = "$Id$";
#endif // !GROBJMGR_H_DEFINED
