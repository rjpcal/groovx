///////////////////////////////////////////////////////////////////////
//
// demangle_cxxfilt.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Feb 27 16:30:08 2003
// written: Wed Mar 19 12:45:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEMANGLE_CXXFILT_H_DEFINED
#define DEMANGLE_CXXFILT_H_DEFINED

#include "util/error.h"
#include "util/pipe.h"
#include "util/strings.h"

#include <string>

#include "util/debug.h"
#include "util/trace.h"

std::string demangle_impl(const std::string& mangled)
{
DOTRACE("demangle_impl");

  fstring command("c++filt ", mangled.c_str());

  Util::Pipe pipe(command.c_str(), "r");

  if (pipe.isClosed())
    {
      throw Util::Error(fstring("while demangling '", mangled.c_str(),
                                "': couldn't open pipe to c++filt"));
    }

  std::string demangled;
  std::getline(pipe.stream(), demangled);

  return demangled;
}

static const char vcid_demangle_cxxfilt_h[] = "$Header$";
#endif // !DEMANGLE_CXXFILT_H_DEFINED
