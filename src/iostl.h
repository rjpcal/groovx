///////////////////////////////////////////////////////////////////////
//
// iostl.h
// Rob Peters
// created: Sat Mar 13 15:20:43 1999
// written: Fri Mar  3 18:28:21 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOSTL_H_DEFINED
#define IOSTL_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#endif

void serializeVecInt(ostream &os, const vector<int>& vec);
void deserializeVecInt(istream &is, vector<int>& vec);
int charCountVecInt(const vector<int>& vec);

static const char vcid_iostl_h[] = "$Header$";
#endif // !IOSTL_H_DEFINED
