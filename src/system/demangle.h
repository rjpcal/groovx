///////////////////////////////////////////////////////////////////////
//
// demangle.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct 13 10:41:03 1999
// written: Wed Oct 13 10:43:39 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEMANGLE_H_DEFINED
#define DEMANGLE_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

string demangle(const string& in);

static const char vcid_demangle_h[] = "$Header$";
#endif // !DEMANGLE_H_DEFINED
