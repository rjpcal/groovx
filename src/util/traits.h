///////////////////////////////////////////////////////////////////////
//
// traits.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri May 18 16:13:27 2001
// written: Fri Jan 18 16:06:54 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRAITS_H_DEFINED
#define TRAITS_H_DEFINED

namespace Util
{
  template <class T>
  struct TypeCue
  {
    typedef T Type;
  };

  template <class T>
  struct TypeTraits
  {
    typedef T DerefT;
    typedef T StackT;
  };

  template <class T>
  struct TypeTraits<T*>
  {
    typedef T Pointee;
    typedef T DerefT;
  };

  template <class T>
  struct TypeTraits<T&>
  {
    typedef T StackT;
  };

  template <class T>
  struct TypeTraits<const T&>
  {
    typedef T StackT;
  };

  template <class T>
  struct StringTraits
  {
    static const char* c_str(T t) { return t.c_str(); }
  };

  template <>
  struct StringTraits<char*>
  {
    static const char* c_str(char* t) { return t; }
  };

  template <>
  struct StringTraits<const char*>
  {
    static const char* c_str(const char* t) { return t; }
  };

  template <unsigned int N>
  struct StringTraits<char[N]>
  {
    static const char* c_str(char t[N]) { return &t[0]; }
  };
}

static const char vcid_traits_h[] = "$Header$";
#endif // !TRAITS_H_DEFINED
