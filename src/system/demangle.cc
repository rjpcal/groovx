///////////////////////////////////////////////////////////////////////
//
// demangle.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct 13 10:41:19 1999
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEMANGLE_CC_DEFINED
#define DEMANGLE_CC_DEFINED

#include "system/demangle.h"

#if defined(NO_TYPENAME_MANGLING)

const char* demangle_cstr(const char* in) { return in; }

#elif defined(__GNUC__)

#if __GNUC__ < 3
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
  return gcc_v3_demangle(in);
}

#endif

#else
#error no method specified for typename demangling
#endif // switch on compiler macro

static const char vcid_demangle_cc[] = "$Header$";
#endif // !DEMANGLE_CC_DEFINED
