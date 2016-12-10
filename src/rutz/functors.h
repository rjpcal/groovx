/** @file rutz/functors.h template mechanisms for determine parameter
    types and return types of functions or general functors */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Sep  7 15:07:16 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_RUTZ_FUNCTORS_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_FUNCTORS_H_UTC20050626084020_DEFINED

namespace rutz
{
  //  #######################################################
  //  =======================================================
  //
  //  func_traits
  //
  //  =======================================================

  /// Placeholder arg type for the this pointer in member functions
  template <class T>
  struct this_pointer
  {};

  /// Holds typedefs for the types of a function's arguments and return value.
  template <class R = void, class... Args>
  struct func_args
  {
    typedef R  retn_t;
    template <size_t N>
    struct arg
    {
      typedef typename std::tuple_element<N, std::tuple<Args...>>::type type;
    };
    static constexpr size_t num_args = sizeof...(Args);
  };

  /// Deduce return type and argument types from operator()s
  template <class Func>
  struct operator_paren_args;

  template <class R, class... Args>
  struct operator_paren_args<R (*)(Args...)>
    :
    public func_args<R, Args...>
  {};

  template <class R, class C, class... Args>
  struct operator_paren_args<R (C::*)(Args...)>
    :
    public func_args<R, Args...>
  {};

  template <class R, class C, class... Args>
  struct operator_paren_args<R (C::*)(Args...) const>
    :
    public func_args<R, Args...>
  {};

  /// A traits class for holding information about functions/functors.
  template <class func>
  struct func_traits
    :
    public operator_paren_args<decltype(&func::operator())>
  {};

  /// func_traits specialization for free functions with any # arguments.
  template <class R, class... Args>
  struct func_traits<R (*)(Args...)>
    :
    public func_args<R, Args...>
  {};

  /// func_traits specialization for non-const member functions with "this" plus any # arguments.
  template <class R, class C, class... Args>
  struct func_traits<R (C::*)(Args...)>
    :
    public func_args<R, this_pointer<C>, Args...>
  {};

  /// func_traits specialization for const member functions with "this" plus any # arguments.
  template <class R, class C, class... Args>
  struct func_traits<R (C::*)(Args...) const>
    :
    public func_args<R, this_pointer<C>, Args...>
  {};

  //  #######################################################
  //  =======================================================

  template <class F>
  class mem_functor;

  /// func_traits specialization for mem_functor.
  template <class MF>
  struct func_traits<mem_functor<MF> > : public func_traits<MF>
  {};

  /// mem_functor adapts a member function to an ordinary operator().
  /** The "this" pointer is passed through the first argument of the
      operator() call, via a raw pointer or a smart pointer. */

  template <class mem_func>
  class mem_functor
  {
  private:
    mem_func m_held_func;

  public:
    mem_functor(mem_func f) : m_held_func(f) {}

    /// Function-call operator for object + any # args.
    template <class ptr, class... Args>
    auto operator()(ptr obj, Args&&... args)
    {
      return ((*obj).*m_held_func)(std::forward<Args>(args)...);
    }
  };


  //  #######################################################
  //  =======================================================

  ///  Traits struct for specifying a "functor" type given a function pointer.
  template <class fptr>
  struct functor_of
  {
    typedef fptr type;
  };

  /// Specialization for any-#-arg non-const mem func.
  template <class R, class C, class... Args>
  struct functor_of< R (C::*)(Args...) >
  {
    typedef rutz::mem_functor<R (C::*)(Args...)> type;
  };

  /// Specialization for any-#-arg const mem func.
  template <class R, class C, class... Args>
  struct functor_of< R (C::*)(Args...) const >
  {
    typedef rutz::mem_functor<R (C::*)(Args...) const> type;
  };

  // ########################################################
  //  =======================================================

  /// Factory function for building a "functor" from any function pointer.
  template <class fptr>
  inline typename functor_of<fptr>::type
  build_functor(fptr f)
  {
    typedef typename functor_of<fptr>::type functor_t;
    return functor_t(f);
  }

} // end namespace rutz

#endif // !GROOVX_RUTZ_FUNCTORS_H_UTC20050626084020_DEFINED
