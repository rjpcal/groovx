///////////////////////////////////////////////////////////////////////
// posmgr.h
// Rob Peters
// created: Wed Apr  7 14:30:27 1999
// written: Wed Apr  7 14:31:47 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POSMGR_H_DEFINED
#define POSMGR_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

class istream;
class Position;

namespace PosMgr {
  Position* newPosition(istream &is, IO::IOFlag flag);
  Position* newPosition(const char* type, istream &is, IO::IOFlag flag);
}

static const char vcid_posmgr_h[] = "$Header$";
#endif // !POSMGR_H_DEFINED
