///////////////////////////////////////////////////////////////////////
//
// traits.h
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri May 18 16:13:27 2001
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

#ifndef TRAITS_H_DEFINED
#define TRAITS_H_DEFINED

namespace rutz
{
  /// Basic type traits class.
  template <class T>
  struct type_traits
  {
    typedef T deref_t;
    typedef T stack_t;
  };

  /// Specialization of type traits for pointers.
  template <class T>
  struct type_traits<T*>
  {
    typedef T pointee_t;
    typedef T deref_t;
  };

  /// Specialization of type traits for references.
  template <class T>
  struct type_traits<T&>
  {
    typedef T stack_t;
  };

  /// Specialization of type traits for const references.
  template <class T>
  struct type_traits<const T&>
  {
    typedef T stack_t;
  };

  /// Select between two types based on a compile-time constant boolean expression.
  template <bool test, class if_true, class if_false>
  struct select_if
  {
    typedef if_true result_t;
  };

  /// Specialization of select_if for 'false'.
  template <class if_true, class if_false>
  struct select_if<false, if_true, if_false>
  {
    typedef if_false result_t;
  };

  /// Helper class for is_sub_super.
  template <class T>
  struct type_match
  {
    /** dummy type */ struct s1 { char x; };
    /** dummy type */ struct s2 { s1 x[2]; };

    static s1 foo(T* p);
    static s2 foo(...);
  };

  /// Determine whether sub derives from super.
  template <class sub, class super>
  struct is_sub_super
  {
    enum { sz = sizeof(type_match<super>::foo(static_cast<sub*>(0))) };

    enum
      {
        result = ((sz == sizeof(typename type_match<super>::s1))
                  ? 1
                  : 0)
      };
  };
}

static const char vcid_traits_h[] = "$Id$ $URL$";
#endif // !TRAITS_H_DEFINED
