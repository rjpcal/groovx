///////////////////////////////////////////////////////////////////////
//
// readutils.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov 16 14:25:40 1999
// written: Tue Nov 16 14:31:18 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READUTILS_CC_DEFINED
#define READUTILS_CC_DEFINED

#include "readutils.h"

#include <strstream.h>

string ReadUtils::makeNumberString(int number) {
  char buf[16];
  ostrstream ost(buf, 16);
  ost << number << '\0';
  return buf;
}

static const char vcid_readutils_cc[] = "$Header$";
#endif // !READUTILS_CC_DEFINED
