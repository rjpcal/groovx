///////////////////////////////////////////////////////////////////////
// ioutils.cc
// Rob Peters 
// created: Jan-99
// written: Mon Apr 19 13:45:34 1999
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

void serializeCstring(ostream &os, const char* str, const char sep) {
  int len = strlen(str);
  os << len << sep << str << sep;
  if (os.fail()) throw OutputError("Cstring");
}

void deserializeCstring(istream &is, char*& str) {
  int len;
  is >> len;
  if (str != NULL) 
    delete [] str;
  str = new char[len+1];
  is.getline(str, len+1);
  if (is.fail()) throw InputError("Cstring");
}

void fatalInputError(const char* type) {
  cerr << "fatal error: bad input while attempting to construct " 
       << type << endl;
  exit(-1);
}

static const char vcid_ioutils_cc[] = "$Header$";
#endif // !IOUTILS_CC_DEFINED
