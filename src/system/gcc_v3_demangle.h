///////////////////////////////////////////////////////////////////////
//
// gcc_v3_demangle.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 19 17:00:38 2001
// written: Mon Jul  2 15:14:31 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GCC_V3_DEMANGLE_H_DEFINED
#define GCC_V3_DEMANGLE_H_DEFINED

#include <cctype>
#include <cstdlib>
#include <string>

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

std::string gcc_v3_demangle(const std::string& in);

class GccV3Demangler {
private:
  std::string itsBuf;
  std::string::size_type itsPos;

  void readName();
  void readNestedName();
  void readUnscopedName();
  void readType();
  std::string readQualifiers();
  void readUserDefinedType();
  void readTemplateArgs();
  void readBuiltinType();
  int munchInt();

public:
  GccV3Demangler(const std::string& in) :
    itsBuf(in), itsPos(0) {}

  std::string demangle()
  {
    readName();
    Assert(itsPos == itsBuf.length());
    return itsBuf;
  }
};

int GccV3Demangler::munchInt() {
  int val = 0;
  while ( isdigit(itsBuf[itsPos]) ) {
    val *= 10;
    val += (itsBuf[itsPos] - '0');
    itsBuf.erase(itsPos, 1);
  }

  return val;
}

void GccV3Demangler::readBuiltinType() {

  char typecode = itsBuf[itsPos];
  itsBuf.erase(itsPos, 1);
  switch (typecode) {
  case 'a':
    itsBuf.insert(itsPos, "signed char"); itsPos += 11;
    break;
  case 'b':
    itsBuf.insert(itsPos, "bool"); itsPos += 4;
    break;
  case 'c':
    itsBuf.insert(itsPos, "char"); itsPos += 4;
    break;
  case 'd':
    itsBuf.insert(itsPos, "double"); itsPos += 6;
    break;
  case 'f':
    itsBuf.insert(itsPos, "float"); itsPos += 5;
    break;
  case 'h':
    itsBuf.insert(itsPos, "unsigned char"); itsPos += 13;
    break;
  case 'i':
    itsBuf.insert(itsPos, "int"); itsPos += 3;
    break;
  case 'j':
    itsBuf.insert(itsPos, "unsigned int"); itsPos += 12;
    break;
  case 'l':
    itsBuf.insert(itsPos, "long"); itsPos += 4;
    break;
  case 'm':
    itsBuf.insert(itsPos, "unsigned long"); itsPos += 13;
    break;
  case 's':
    itsBuf.insert(itsPos, "short"); itsPos += 5;
    break;
  case 't':
    itsBuf.insert(itsPos, "unsigned short"); itsPos += 14;
    break;
  case 'v':
    itsBuf.insert(itsPos, "void"); itsPos += 4;
    break;
  default:
    DebugEvalNL(typecode);
    Assert(0);
    break;
  }
}

void GccV3Demangler::readTemplateArgs() {
  Assert(itsBuf[itsPos] == 'I');
  itsBuf.erase(itsPos, 1);
  itsBuf.insert(itsPos, "<");
  itsPos += 1;
  while (true)
    {
      readName();

      if (itsBuf[itsPos] == 'E')
        {
          itsBuf.erase(itsPos, 1);
          itsBuf.insert(itsPos, ">");
          itsPos += 1;
          return;
        }
      else
        {
          itsBuf.insert(itsPos, ", ");
          itsPos += 2;
        }
    }
}

void GccV3Demangler::readUserDefinedType() {
  int length = munchInt();
  itsPos += length;

  if (itsBuf[itsPos] == 'I')
    {
      readTemplateArgs();
    }
}

void GccV3Demangler::readType() {
  std::string qual = readQualifiers();

  // Check if it's a user-defined type...
  if ( isdigit(itsBuf[itsPos]) )
    {
      readUserDefinedType();
    }
  // ...or a builtin type
  else
    {
      readBuiltinType();
    }

  itsBuf.insert(itsPos, qual);
  itsPos += qual.length();
}

std::string GccV3Demangler::readQualifiers() {
  std::string qual;
  while (true) {
    switch (itsBuf[itsPos]) {
    case 'P':
      qual.insert(0, "*");
      itsBuf.erase(itsPos, 1);
      break;
    case 'R':
      qual.insert(0, "&");
      itsBuf.erase(itsPos, 1);
      break;
    case 'V':
      qual.insert(0, " volatile");
      itsBuf.erase(itsPos, 1);
      break;
    case 'K':
      qual.insert(0, " const");
      itsBuf.erase(itsPos, 1);
      break;
    default:
      return qual;
    }
  }

  return qual;
}

void GccV3Demangler::readUnscopedName() {
  if ( itsBuf[itsPos] == 'S')
    {
      Assert(itsBuf[itsPos+1] == 't');
      itsBuf.erase(itsPos, 2);

      itsBuf.insert(itsPos, "std::");

      itsPos += 5;
    }

  readType();
}

void GccV3Demangler::readNestedName() {
  Assert(itsBuf[itsPos] == 'N');
  itsBuf.erase(itsPos, 1);
  while (true) {
    readUnscopedName();

    // See if we're done nesting...
    if (itsBuf[itsPos] == 'E') {
      itsBuf.erase(itsPos, 1);
      return;
    }

    // ...otherwise insert a scope resolution operator ("::")
    itsBuf.insert(itsPos, "::");
    // Skip over the just-inserted "::"
    itsPos += 2;
  }
}

void GccV3Demangler::readName() {
  // Check if we have namespace qualifiers...
  if (itsBuf[itsPos] == 'N')
    {
      readNestedName();
    }
  else
    {
      readUnscopedName();
    }
}

std::string gcc_v3_demangle(const std::string& in) {
DOTRACE("gcc_v3_demangle");

  GccV3Demangler dm(in);

  return dm.demangle();
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

//  int main() {
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
