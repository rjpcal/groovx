///////////////////////////////////////////////////////////////////////
// io.cc
// Rob Peters
// created: Tue Mar  9 20:25:02 1999
// written: Tue Mar 16 19:40:47 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IO_CC_DEFINED
#define IO_CC_DEFINED

#include "io.h"

#include <iostream.h>

IOResult checkStream(ostream &os) {
  return (!os.bad() ? IO_OK : IO_ERROR);
}

IOResult checkStream(istream &is) {
  return (!is.bad() ? IO_OK : IO_ERROR);
}

IOResult IO::deserialize(istream &is, IOFlag flag) { return IO_OK; }

static const char vcid_io_cc[] = "$Header$";
#endif // !IO_CC_DEFINED
