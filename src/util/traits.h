///////////////////////////////////////////////////////////////////////
//
// traits.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri May 18 16:13:27 2001
// written: Tue Jul  2 13:10:18 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRAITS_H_DEFINED
#define TRAITS_H_DEFINED

namespace Util
{
  /// Symbol class for specifying some type.
  template <class T>
  struct TypeCue
  {
    typedef T Type;
  };

  /// Basic type traits class.
  template <class T>
  struct TypeTraits
  {
    typedef T DerefT;
    typedef T StackT;
  };

  /// Specialization of type traits for pointers.
  template <class T>
  struct TypeTraits<T*>
  {
    typedef T Pointee;
    typedef T DerefT;
  };

  /// Specialization of type traits for references.
  template <class T>
  struct TypeTraits<T&>
  {
    typedef T StackT;
  };

  /// Specialization of type traits for const references.
  template <class T>
  struct TypeTraits<const T&>
  {
    typedef T StackT;
  };

  /// Basic string class.
  template <class T>
  struct StringTraits
  {
    static const char* c_str(T t) { return t.c_str(); }
  };

  /// Specialization of string traits for char*.
  template <>
  struct StringTraits<char*>
  {
    static const char* c_str(char* t) { return t; }
  };

  /// Specialization of string traits for const char*.
  template <>
  struct StringTraits<const char*>
  {
    static const char* c_str(const char* t) { return t; }
  };

  /// Specialization of string traits for char array.
  template <unsigned int N>
  struct StringTraits<char[N]>
  {
    static const char* c_str(char t[N]) { return &t[0]; }
  };
}

static const char vcid_traits_h[] = "$Header$";
#endif // !TRAITS_H_DEFINED
