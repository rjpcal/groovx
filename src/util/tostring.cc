///////////////////////////////////////////////////////////////////////
//
// tostring.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sun Aug  5 20:12:30 2001
// written: Wed Mar 19 12:45:35 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOSTRING_CC_DEFINED
#define TOSTRING_CC_DEFINED

#include "util/tostring.h"

#include <cstdio>
#include <cstring>

namespace
{
  const int SZ = 64;
  static char buf[SZ];
}

Util::CharData::CharData(const char* t) :
  text(t), len(t ? strlen(t) : 0)
{}

const char* Util::num2str(bool x)         { snprintf(buf, SZ, "%d", int(x)); return buf; }
const char* Util::num2str(int x)          { snprintf(buf, SZ, "%d", x); return buf; }
const char* Util::num2str(unsigned int x) { snprintf(buf, SZ, "%u", x); return buf; }
const char* Util::num2str(long x)         { snprintf(buf, SZ, "%ld", x); return buf; }
const char* Util::num2str(unsigned long x){ snprintf(buf, SZ, "%lu", x); return buf; }
const char* Util::num2str(double x)       { snprintf(buf, SZ, "%g", x); return buf; }

static const char vcid_tostring_cc[] = "$Header$";
#endif // !TOSTRING_CC_DEFINED
