///////////////////////////////////////////////////////////////////////
// io.cc
// Rob Peters
// created: Tue Mar  9 20:25:02 1999
// written: Mon Apr 26 14:32:20 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef IO_CC_DEFINED
#define IO_CC_DEFINED

#include "io.h"

#include <typeinfo>

#define NO_TRACE
#include "trace.h"

IoError::IoError(const string& str) :
  itsInfo(string(typeid(*this).name()) + ": " + str)
{
DOTRACE("IoError::IoError(const string&)");
}

IoError::IoError(const type_info& ti) :
  itsInfo(string(typeid(*this).name()) + ": " + ti.name())
{
DOTRACE("IoError::IoError(const type_info&");
}

void IoError::setInfo(const string& str) {
DOTRACE("IoError::setInfo(const string&)");
  itsInfo = string(typeid(*this).name()) + ": " + str;
}

void IoError::setInfo(const type_info& ti) {
DOTRACE("IoError::setInfo(const type_info&");
  itsInfo = string(typeid(*this).name()) + ": " + ti.name();
}

static const char vcid_io_cc[] = "$Header$";
#endif // !IO_CC_DEFINED
