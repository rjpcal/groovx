///////////////////////////////////////////////////////////////////////
//
// demangle.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct 13 10:41:19 1999
// written: Sat Sep  8 08:54:51 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEMANGLE_CC_DEFINED
#define DEMANGLE_CC_DEFINED

#include "system/demangle.h"

#if defined(MIPSPRO_COMPILER)

const char* demangle_cstr(const char* in) { return in; }

#elif defined(GCC_COMPILER)

#if GCC_COMPILER < 3
#  include "system/gcc_v2_demangle.h"

const char* demangle_cstr(const char* in)
{
  static std::string result;
  result = gcc_v2_demangle(in);
  return result.c_str();
}

#else
#  include "system/gcc_v3_demangle.h"

const char* demangle_cstr(const char* in)
{
  static std::string result;
  result = gcc_v3_demangle(in);
  return result.c_str();
}

#endif

#else
#error no compiler macro is defined
#endif // switch on compiler macro

static const char vcid_demangle_cc[] = "$Header$";
#endif // !DEMANGLE_CC_DEFINED
