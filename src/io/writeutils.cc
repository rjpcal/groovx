///////////////////////////////////////////////////////////////////////
//
// writeutils.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov 16 14:21:32 1999
// written: Thu Mar 30 12:14:49 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITEUTILS_CC_DEFINED
#define WRITEUTILS_CC_DEFINED

#include "io/writeutils.h"

#include "util/strings.h"

#include <strstream.h>

namespace {

const char* makeNumberString(int number) {
  static char buf[32];
  ostrstream ost(buf, 16);
  ost << number << '\0';
  return buf;
}

}

const char* IO::WriteUtils::makeElementNameString(const char* seq_name,
															 int element_num) {
  static dynamic_string result("");
  result = seq_name;
  result += makeNumberString(element_num);
  return result.c_str();
}

const char* IO::WriteUtils::makeSeqCountString(const char* seq_name) {
  static dynamic_string result("");
  result = seq_name;
  result += "Count";
  return result.c_str();
}

static const char vcid_writeutils_cc[] = "$Header$";
#endif // !WRITEUTILS_CC_DEFINED
