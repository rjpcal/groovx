///////////////////////////////////////////////////////////////////////
// iostl.cc
// Rob Peters
// created: Sat Mar 13 15:21:33 1999
// written: Tue Mar 16 19:40:14 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IOSTL_CC_DEFINED
#define IOSTL_CC_DEFINED

#include "iostl.h"

#include <iostream.h>
#include <string>

IOResult serializeVecInt(ostream &os, const vector<int>& vec) {
  char sep = ' ';
  os << vec.size() << sep << sep;
  for (int i = 0; i < vec.size(); i++) {
    os << vec[i] << sep;
  }
  return checkStream(os);
}

IOResult deserializeVecInt(istream &is, vector<int>& vec) {
  int size;
  is >> size;
  vec.resize(size, 0);
  for (int i = 0; i < size; i++) {
    is >> vec[i];
  }
  return checkStream(is);
}

static const char vcid_iostl_cc[] = "$Header$";
#endif // !IOSTL_CC_DEFINED
