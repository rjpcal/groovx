///////////////////////////////////////////////////////////////////////
//
// demangle_gcc_v3.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jun 19 17:00:38 2001 (as gcc_v3_demangle.h)
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

#ifndef DEMANGLE_GCC_V3_H_DEFINED
#define DEMANGLE_GCC_V3_H_DEFINED

#include "util/error.h"

#include <cstdlib>
#include <cxxabi.h>
#include <string>

#include "util/debug.h"
#include "util/trace.h"

namespace
{
  std::string demangle_gcc_v3(const std::string& mangled)
  {
    DOTRACE("demangle_gcc_v3");

    int status = 0;

    static std::size_t length = 256;
    static char* demangled = static_cast<char*>(malloc(length));

    demangled = abi::__cxa_demangle(mangled.c_str(), demangled,
                                    &length, &status);

    if (status == 0)
      {
        ASSERT(demangled != 0);
        return demangled;
      }

    fstring msg("while demangling '", mangled.c_str(), "': ");

    switch (status)
      {
      case -1:
        throw rutz::error(fstring(msg, "memory allocation error"),
                          SRC_POS);
        break;
      case -2:
        throw rutz::error(fstring(msg, "invalid mangled name"),
                          SRC_POS);
        break;
      case -3:
        throw rutz::error(fstring(msg, "invalid arguments (e.g. "
                                  "buf non-NULL and length NULL'"),
                          SRC_POS);
        break;
      default:
        throw rutz::error(fstring(msg, "unknown error code"), SRC_POS);
      }

    ASSERT(false);
    return "can't happen"; // can't happen, but placate compiler
  }
}

//
// demangling test code
//

//  #include <iostream>
//  #include <exception>
//  #include <typeinfo>

//  #include "src/util/demangle_gcc_v3.h"

//  void printboth(const char* n)
//  {
//    std::cout << n << '\t' << gcc_v3_demangle(n) << std::endl;
//  }

//  class Global { int x; };

//  namespace Nspace { struct Nested { struct Inner {int x; }; }; }

//  template <typename T>
//  class Tplate { T x; };

//  class i {};

//  template <class T1, class T2>
//  class Tplate2 { T1 x1; T2 x2; };

//  template <class T1>
//  class Tplate3 { struct Xt {}; };

//  int main()
//  {
//    printboth(typeid(int).name());
//    printboth(typeid(double).name());
//    printboth(typeid(i).name());
//    printboth(typeid(Global).name());
//    printboth(typeid(std::exception).name());
//    printboth(typeid(Nspace::Nested).name());
//    printboth(typeid(Nspace::Nested::Inner).name());
//    printboth(typeid(Tplate<int>).name());
//    printboth(typeid(Tplate<double>).name());
//    printboth(typeid(Tplate<Global>).name());
//    printboth(typeid(Tplate<std::exception>).name());
//    printboth(typeid(Tplate<Nspace::Nested::Inner>).name());
//    printboth(typeid(Tplate2<int, double>).name());
//    printboth(typeid(Tplate<int*>).name());
//    printboth(typeid(Tplate<Global*>).name());
//    printboth(typeid(Tplate<const int*>).name());
//    printboth(typeid(Tplate<const Global*>).name());
//    printboth(typeid(Tplate<int* const>).name());
//    printboth(typeid(Tplate<Global* const>).name());
//    printboth(typeid(Tplate<int const* const>).name());
//    printboth(typeid(Tplate<Global const* const>).name());
//    printboth(typeid(Tplate3<int>::Xt).name());
//  }

static const char vcid_demangle_gcc_v3_h[] = "$Header$";
#endif // !DEMANGLE_GCC_V3_H_DEFINED
