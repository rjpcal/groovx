///////////////////////////////////////////////////////////////////////
//
// readutils.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov 16 14:25:40 1999
// written: Wed Mar 15 10:20:36 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READUTILS_CC_DEFINED
#define READUTILS_CC_DEFINED

#include "readutils.h"

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

const char* ReadUtils::makeElementNameString(const char* seq_name,
															 int element_num) {
  static dynamic_string result("");
  result = seq_name;
  result += makeNumberString(element_num);
  return result.c_str();
}

const char* ReadUtils::makeSeqCountString(const char* seq_name) {
  static dynamic_string result("");
  result = seq_name;
  result += "Count";
  return result.c_str();
}

static const char vcid_readutils_cc[] = "$Header$";
#endif // !READUTILS_CC_DEFINED
