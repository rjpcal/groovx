///////////////////////////////////////////////////////////////////////
// ioutils.h
// Rob Peters
// created: Jan-99
// written: Fri Mar 12 11:30:03 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef IOUTILS_H_DEFINED
#define IOUTILS_H_DEFINED

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

IOResult serializeCstring(ostream &os, const char* str, const char sep='\t');
void deserializeCstring(istream &is, char*& str, char *type);

IOResult serializeInt(ostream &os, int i, const char sep='\t');
void deserializeInt(istream &is, int &i, char *type);

IOResult serializeDouble(ostream &os, double i, const char sep='\t');
void deserializeDouble(istream &is, double &i, char *type);

IOResult serializeFloat(ostream &os, float i, const char sep='\t');
void deserializeFloat(istream &is, float &i, char *type);

void fatalInputError(const char* type);

#endif // !IOUTILS_H_DEFINED
