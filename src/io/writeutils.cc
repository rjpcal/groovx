///////////////////////////////////////////////////////////////////////
//
// writeutils.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov 16 14:21:32 1999
// written: Fri Mar  3 23:44:40 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITEUTILS_CC_DEFINED
#define WRITEUTILS_CC_DEFINED

#include "writeutils.h"

#include <string>
#include <strstream.h>

string WriteUtils::makeNumberString(int number) {
  char buf[16];
  ostrstream ost(buf, 16);
  ost << number << '\0';
  return buf;
}

const char* WriteUtils::makeElementNameString(const char* seq_name,
															 int element_num) {
  static string result;
  result = seq_name;
  result += makeNumberString(element_num);
  return result.c_str();
}

const char* WriteUtils::makeSeqCountString(const char* seq_name) {
  static string result;
  result = seq_name;
  result += "Count";
  return result.c_str();
}

static const char vcid_writeutils_cc[] = "$Header$";
#endif // !WRITEUTILS_CC_DEFINED
