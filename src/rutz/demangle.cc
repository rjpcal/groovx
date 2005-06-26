///////////////////////////////////////////////////////////////////////
//
// demangle.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct 13 10:41:19 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_RUTZ_DEMANGLE_CC_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_DEMANGLE_CC_UTC20050626084020_DEFINED

#include "rutz/demangle.h"

#if defined(NO_TYPENAME_MANGLING)

namespace
{
  const char* demangle_impl(const char* in) { return in; }
}

#  define DEMANGLE_IMPL demangle_impl

#else

// Here we pick the appropriate system-dependent header that defines a
// function for demangling mangled names with the following prototype:

//   std::string demangle_impl(const std::string& mangled)

#  if defined(HAVE_CXXABI_H)
#    include "rutz/demangle_gcc_v3.h"
#    define DEMANGLE_IMPL demangle_gcc_v3
#  elif defined(__GNUC__) && __GNUC__ < 3
#    include "rutz/demangle_gcc_v2.h"
#    define DEMANGLE_IMPL demangle_gcc_v2
#  elif defined(HAVE_PROG_CXXFILT)
#    include "rutz/demangle_cxxfilt.h"
#    define DEMANGLE_IMPL demangle_cxxfilt
#  else
// use the cxxabi demangler by default
#    include "rutz/demangle_gcc_v3.h"
#    define DEMANGLE_IMPL demangle_gcc_v3
#  endif
#endif // defined(NO_TYPENAME_MANGLING)

#include <map>
#include <string>

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

namespace
{
  // why can't we make this a map<type_info, string>?
  //   (1) gcc libstdc++ doesn't seem to have type_info::operator<()
  //   (2) gcc libstdc++ doesn't allow copying of type_info objects
  typedef std::map<std::string, std::string> Cache;
  Cache nameCache;
}

const char* rutz::demangled_name(const std::type_info& info)
{
GVX_TRACE("rutz::demangled_name");

  const std::string mangled = info.name();

  Cache::iterator itr = nameCache.find(mangled);

  if (itr != nameCache.end())
    {
      return (*itr).second.c_str();
    }

  const std::string demangled = DEMANGLE_IMPL(info.name());

  std::pair<Cache::iterator, bool> result =
    nameCache.insert(Cache::value_type(mangled, demangled));

  GVX_ASSERT(result.second == true);

  return (*result.first).second.c_str();
}

static const char vcid_groovx_rutz_demangle_cc_utc20050626084020[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_DEMANGLE_CC_UTC20050626084020_DEFINED
