///////////////////////////////////////////////////////////////////////
// iostl.cc
// Rob Peters
// created: Sat Mar 13 15:21:33 1999
// written: Sat Mar 13 15:25:58 1999
///////////////////////////////////////////////////////////////////////

#ifndef IOSTL_CC_DEFINED
#define IOSTL_CC_DEFINED

#include "iostl.h"

#include <iostream.h>
#include <string>

IOResult serializeVecInt(ostream &os, const vector<int>& vec) {
  char sep = ' ';
  os << vec.size() << endl;
  for (int i = 0; i < vec.size()-1; i++) { // all but the last element
	 os << vec[i] << sep;
  }
  os << vec[vec.size()-1] << endl; // put the last element with an endl
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

static const char vcid_iostl_cc[] = "$Id$";
#endif // !IOSTL_CC_DEFINED
