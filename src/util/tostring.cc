///////////////////////////////////////////////////////////////////////
//
// tostring.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Aug  5 20:12:30 2001
// written: Mon Aug 20 12:09:40 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOSTRING_CC_DEFINED
#define TOSTRING_CC_DEFINED

#include "util/tostring.h"

#include <cstring>
#include <strstream.h>

#include "util/trace.h"

namespace
{
  static char num2str_buf[64];
}

Util::CharData::CharData(const char* t) :
  text(t), len(t ? strlen(t) : 0)
{}

template <class T>
const char* Util::num2str(const T& x)
{
DOTRACE("Util::num2str");
  ostrstream ost(num2str_buf, 30);
  ost << x << '\0';
  return &num2str_buf[0];
}

#ifndef ACC_COMPILER
template const char* Util::num2str<int>(const int&);
template const char* Util::num2str<unsigned int>(const unsigned int&);
template const char* Util::num2str<long>(const long&);
template const char* Util::num2str<unsigned long>(const unsigned long&);
template const char* Util::num2str<double>(const double&);
#else
namespace
{
  void dummy() // just a hack since aCC won't do explicit instantiation
  {
    num2str((int)0);
    num2str((unsigned int)0);
    num2str((long)0);
    num2str((unsigned long)0);
    num2str((double)0);
  }
}
#endif

static const char vcid_tostring_cc[] = "$Header$";
#endif // !TOSTRING_CC_DEFINED
