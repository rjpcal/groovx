///////////////////////////////////////////////////////////////////////
//
// demangle_gcc_v3.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 19 17:00:38 2001 (as gcc_v3_demangle.h)
// written: Thu Feb 27 16:27:42 2003
// $Id$
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

std::string demangle_impl(const std::string& mangled)
{
DOTRACE("demangle_impl");

  int status = 0;

  static unsigned int length = 256;
  static char* demangled = static_cast<char*>(malloc(length));

  demangled = abi::__cxa_demangle(mangled.c_str(), demangled, &length, &status);

  if (status == 0)
    {
      Assert(demangled != 0);
      return demangled;
    }

  fstring msg("while demangling '", mangled.c_str(), "': ");

  switch (status)
    {
    case -1:
      throw Util::Error(fstring(msg, "memory allocation error"));
      break;
    case -2:
      throw Util::Error(fstring(msg, "invalid mangled name"));
      break;
    case -3:
      throw Util::Error(fstring(msg, "invalid arguments"
                                " (e.g. buf non-NULL and length NULL'"));
      break;
    default:
      throw Util::Error(fstring(msg, "unknown error code"));
    }

  Assert(false);
  return "can't happen"; // can't happen, but placate compiler
}

//
// demangling test code
//

//  #include <iostream>
//  #include <exception>
//  #include <typeinfo>

//  #include "src/system/gcc_v3_demangle.h"

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