///////////////////////////////////////////////////////////////////////
//
// traits.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri May 18 16:13:27 2001
// written: Mon Jan 13 11:08:25 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRAITS_H_DEFINED
#define TRAITS_H_DEFINED

namespace Util
{
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

  /// Select between two types based on a compile-time constant boolean expression.
  template <bool test, class IfTrue, class IfFalse>
  struct SelectIf
  {
    typedef IfTrue Type;
  };

  /// Specialization of SelectIf for 'false'.
  template <class IfTrue, class IfFalse>
  struct SelectIf<false, IfTrue, IfFalse>
  {
    typedef IfFalse Type;
  };

  /// Helper class for IsSubSuper.
  template <class T>
  struct TypeMatch
  {
    struct s1 { char x; };
    struct s2 { s1 x[2]; };

    static s1 foo(T* p);
    static s2 foo(...);
  };

  /// Determine whether Sub derives from Super.
  template <class Sub, class Super>
  struct IsSubSuper
  {
    enum { sz = sizeof(TypeMatch<Super>::foo((Sub*)0)) };
    enum { result = (sz == sizeof(typename TypeMatch<Super>::s1)) ? 1 : 0 };
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
