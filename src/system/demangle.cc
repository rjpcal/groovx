///////////////////////////////////////////////////////////////////////
//
// demangle.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct 13 10:41:19 1999
// written: Wed Oct 13 11:02:18 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEMANGLE_CC_DEFINED
#define DEMANGLE_CC_DEFINED

#include "demangle.h"

#ifdef ACC_COMPILER
string demangle(const string& in) { return in; }
#endif

#ifdef GCC_COMPILER
#include "gccdemangle.cc"
#endif

static const char vcid_demangle_cc[] = "$Header$";
#endif // !DEMANGLE_CC_DEFINED
