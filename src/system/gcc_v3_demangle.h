///////////////////////////////////////////////////////////////////////
//
// gcc_v3_demangle.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 19 17:00:38 2001
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GCC_V3_DEMANGLE_H_DEFINED
#define GCC_V3_DEMANGLE_H_DEFINED

#include "util/error.h"

#include <cstdlib>
#include <cxxabi.h>

#include "util/trace.h"

const char* gcc_v3_demangle(const char* in)
{
DOTRACE("gcc_v3_demangle");

  int status = 0;

  static unsigned int length = 256;
  static char* demangled = static_cast<char*>(malloc(length));

  demangled = abi::__cxa_demangle(in, demangled, &length, &status);

  if (status == 0) return demangled;

  switch (status)
    {
    case -1:
      throw Util::Error(fstring("memory allocation error in demangling of '",
                                in, "'"));
      break;
    case -2:
      throw Util::Error(fstring("invalid mangled name '", in, "'"));
      break;
    case -3:
      throw Util::Error(fstring("invalid arguments in demangling of '",
                                in, "'"));
      break;
    default:
      throw Util::Error(fstring("unknown error code in demangling of '",
                                in, "'"));
    }

  return "can't happen";
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

static const char vcid_gcc_v3_demangle_h[] = "$Header$";
#endif // !GCC_V3_DEMANGLE_H_DEFINED
