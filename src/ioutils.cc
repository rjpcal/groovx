///////////////////////////////////////////////////////////////////////
//
// ioutils.cc
// Rob Peters 
// created: Jan-99
// written: Tue May 25 13:56:59 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOUTILS_CC_DEFINED
#define IOUTILS_CC_DEFINED

#include "ioutils.h"

#include <iostream.h>
#include <cstdlib>              // for exit()
#include <cstring>
#include <string>

#include "io.h"

///////////////////////////////////////////////////////////////////////
//
// IOUtils definitions
//
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

static const char vcid_ioutils_cc[] = "$Header$";
#endif // !IOUTILS_CC_DEFINED
