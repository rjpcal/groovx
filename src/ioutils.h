///////////////////////////////////////////////////////////////////////
//
// ioutils.h
// Rob Peters
// created: Jan-99
// written: Tue May 25 13:56:36 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOUTILS_H_DEFINED
#define IOUTILS_H_DEFINED

class ostream;
class istream;

void serializeCstring(ostream &os, const char* str, const char sep='\t');
void deserializeCstring(istream &is, char*& str);

static const char vcid_ioutils_h[] = "$Header$";
#endif // !IOUTILS_H_DEFINED
