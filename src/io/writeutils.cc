///////////////////////////////////////////////////////////////////////
//
// writeutils.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov 16 14:21:32 1999
// written: Tue Nov 16 14:21:56 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITEUTILS_CC_DEFINED
#define WRITEUTILS_CC_DEFINED

#include "writeutils.h"

#include <strstream.h>

string WriteUtils::makeNumberString(int number) {
  char buf[16];
  ostrstream ost(buf, 16);
  ost << number << '\0';
  return buf;
}

static const char vcid_writeutils_cc[] = "$Header$";
#endif // !WRITEUTILS_CC_DEFINED
