///////////////////////////////////////////////////////////////////////
// grobjmgr.h
// Rob Peters
// created: Mon Mar  8 00:33:26 1999
// written: Tue Mar 16 19:41:44 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef GROBJMGR_H_DEFINED
#define GROBJMGR_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

class istream;
class GrObj;

namespace GrobjMgr {
  GrObj* newGrobj(istream &is, IO::IOFlag flag);
  GrObj* newGrobj(const char* type, istream &is, IO::IOFlag flag);
}

static const char vcid_grobjmgr_h[] = "$Header$";
#endif // !GROBJMGR_H_DEFINED
