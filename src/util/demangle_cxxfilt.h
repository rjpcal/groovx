///////////////////////////////////////////////////////////////////////
//
// demangle_cxxfilt.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Feb 27 16:30:08 2003
// commit: $Id$
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

#ifndef DEMANGLE_CXXFILT_H_DEFINED
#define DEMANGLE_CXXFILT_H_DEFINED

#include "util/error.h"
#include "util/fstring.h"
#include "util/pipe.h"

#include <string>

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

namespace
{
  std::string demangle_cxxfilt(const std::string& mangled)
  {
    DOTRACE("demangle_cxxfilt");

    rutz::fstring command("c++filt ", mangled.c_str());

    rutz::shell_pipe pipe(command.c_str(), "r");

    if (pipe.is_closed())
      {
        throw rutz::error(rutz::fstring("while demangling '",
                                        mangled.c_str(),
                                        "': couldn't open pipe to c++filt"),
                          SRC_POS);
      }

    std::string demangled;
    std::getline(pipe.stream(), demangled);

    return demangled;
  }
}

static const char vcid_demangle_cxxfilt_h[] = "$Id$ $URL$";
#endif // !DEMANGLE_CXXFILT_H_DEFINED
