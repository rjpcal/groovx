///////////////////////////////////////////////////////////////////////
// iostl.h
// Rob Peters
// created: Sat Mar 13 15:20:43 1999
// written: Thu Apr  8 18:11:17 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IOSTL_H_DEFINED
#define IOSTL_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

#include <vector>

void serializeVecInt(ostream &os, const vector<int>& vec);
void deserializeVecInt(istream &is, vector<int>& vec);

static const char vcid_iostl_h[] = "$Header$";
#endif // !IOSTL_H_DEFINED
