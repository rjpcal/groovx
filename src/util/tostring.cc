///////////////////////////////////////////////////////////////////////
//
// tostring.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Aug  5 20:12:30 2001
// written: Fri Feb  1 10:57:49 2002
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

  template <class T>
  const char* num2str_impl(const T& x)
  {
    ostrstream ost(num2str_buf, 30);
    ost << x << '\0';
    return &num2str_buf[0];
  }
}

Util::CharData::CharData(const char* t) :
  text(t), len(t ? strlen(t) : 0)
{}

const char* Util::num2str(bool x) { return num2str_impl(x); }
const char* Util::num2str(int x) { return num2str_impl(x); }
const char* Util::num2str(unsigned int x) { return num2str_impl(x); }
const char* Util::num2str(long x) { return num2str_impl(x); }
const char* Util::num2str(unsigned long x) { return num2str_impl(x); }
const char* Util::num2str(double x) { return num2str_impl(x); }

static const char vcid_tostring_cc[] = "$Header$";
#endif // !TOSTRING_CC_DEFINED
