///////////////////////////////////////////////////////////////////////
//
// demangle.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Oct 13 10:41:19 1999
// written: Wed Mar 19 12:45:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEMANGLE_CC_DEFINED
#define DEMANGLE_CC_DEFINED

#include "system/demangle.h"

#if defined(NO_TYPENAME_MANGLING)

const char* demangle_cstr(const char* in) { return in; }

#else

// Here we pick the appropriate system-dependent header that defines a
// function for demangling mangled names with the following prototype:

//   std::string demangle_impl(const std::string& mangled)

#  if defined(__GNUC__)
#    if __GNUC__ < 3
#      include "system/demangle_gcc_v2.h"
#    else
#      include "system/demangle_gcc_v3.h"
#    endif
#  elif defined(HAVE_PROG_CXXFILT)
#    include "system/demangle_cxxfilt.h"
#  else
#    error no method specified for typename demangling
#  endif

#include <map>
#include <string>

#include "util/debug.h"
#include "util/trace.h"

namespace
{
  typedef std::map<std::string, std::string> Cache;
  Cache nameCache;
}

const char* demangle_cstr(const char* in)
{
DOTRACE("demangle_cstr");

  const std::string mangled = in;

  Cache::iterator itr = nameCache.find(in);

  if (itr != nameCache.end())
    {
      return (*itr).second.c_str();
    }

  const std::string demangled = demangle_impl(in);

  std::pair<Cache::iterator, bool> result =
    nameCache.insert(Cache::value_type(in, demangled));

  Assert(result.second == true);

  return (*result.first).second.c_str();
}

#endif // !defined(NO_TYPENAME_MANGLING)

static const char vcid_demangle_cc[] = "$Header$";
#endif // !DEMANGLE_CC_DEFINED
