///////////////////////////////////////////////////////////////////////
// ioutils.cc
// Rob Peters 
// created: Jan-99
// written: Tue Mar 16 19:39:45 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IOUTILS_CC_DEFINED
#define IOUTILS_CC_DEFINED

#include "ioutils.h"

#include <iostream.h>
#include <cstdlib>              // for exit()
#include <cstring>
#include <string>

///////////////////////////////////////////////////////////////////////
// IOUtils definitions
///////////////////////////////////////////////////////////////////////

IOResult serializeCstring(ostream &os, const char* str, const char sep) {
  int len = strlen(str);
  os << len << sep << str << sep;
  return checkStream(os);
}

IOResult deserializeCstring(istream &is, char*& str) {
  int len;
  is >> len;
  if (str != NULL) 
    delete [] str;
  str = new char[len+1];
  is >> str;  
  return checkStream(is);
}

void fatalInputError(const char* type) {
  cerr << "fatal error: bad input while attempting to construct " 
       << type << endl;
  exit(-1);
}

static const char vcid_ioutils_cc[] = "$Header$";
#endif // !IOUTILS_CC_DEFINED
