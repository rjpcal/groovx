///////////////////////////////////////////////////////////////////////
// ioutils.h
// Rob Peters
// created: Jan-99
// written: Tue Mar 16 19:39:29 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IOUTILS_H_DEFINED
#define IOUTILS_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

IOResult serializeCstring(ostream &os, const char* str, const char sep='\t');
IOResult deserializeCstring(istream &is, char*& str);

void fatalInputError(const char* type);

static const char vcid_ioutils_h[] = "$Header$";
#endif // !IOUTILS_H_DEFINED
