///////////////////////////////////////////////////////////////////////
// ioutils.cc
// Rob Peters 
// created: Jan-99
// written: Tue Mar  9 20:20:48 1999
///////////////////////////////////////////////////////////////////////

#ifndef IOUTILS_CC_DEFINED
#define IOUTILS_CC_DEFINED

#include "ioutils.h"

#include <iostream.h>
#include <cstdlib>				  // for exit()
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

void deserializeCstring(istream &is, char*& str, char* type) {
  int len;
  is >> len;
  if (str != NULL) 
	 delete [] str;
  str = new char[len+1];
  is >> str;  
  if (checkStream(is) != IO_OK) fatalInputError(type);
}

IOResult serializeInt(ostream &os, int i, const char sep) {
  os << i << sep;
  return checkStream(os);
}

void deserializeInt(istream &is, int &i, char *type) {
  is >> i;
  if (checkStream(is) != IO_OK) fatalInputError(type);
}

IOResult serializeDouble(ostream &os, double d, const char sep) {
  os << d << sep;
  return checkStream(os);
}

void deserializeDouble(istream &is, double &d, char *type) {
  is >> d;
  if (checkStream(is) != IO_OK) fatalInputError(type);
}

IOResult serializeFloat(ostream &os, float f, const char sep) {
  os << f << sep;
  return checkStream(os);
}

void deserializeFloat(istream &is, float &f, char *type) {
  is >> f;
  if (checkStream(is) != IO_OK) fatalInputError(type);
}


void fatalInputError(const char* type) {
  cerr << "fatal error: bad input while attempting to construct " 
		 << type << endl;
  exit(-1);
}

#endif // !IOUTILS_CC_DEFINED
