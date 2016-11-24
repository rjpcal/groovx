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
      typedef typename std::tuple_element<N-1, std::tuple<Args...>>::type type;
    };
  };

  /// A traits class for holding information about functions/functors.
  template <class func>
  struct func_traits
  {
    typedef typename func::retn_t retn_t;
  };

  //  =======================================================
  //
  //  func_traits specializations for free functions
  //
  //  =======================================================

  /// Specialization for free functions with any # arguments.
  template <class R, class... Args>
  struct func_traits<R (*)(Args...)>
    :
    public func_args<R, Args...>
  {
    static constexpr int num_args = sizeof...(Args);
  };

  //  =======================================================
  //
  //  func_traits specializations for member functions
  //
  //  We treat the "this" pointer as an implicit first-argument to the
  //  function.
  //
  //  =======================================================

  /// Specialization for non-const member functions with "this" plus any # arguments.
  template <class R, class C, class... Args>
  struct func_traits<R (C::*)(Args...)>
    :
    public func_args<R, this_pointer<C>, Args...>
  {
    static constexpr int num_args = sizeof...(Args) + 1;
  };

  /// Specialization for const member functions with "this" plus any # arguments.
  template <class R, class C, class... Args>
  struct func_traits<R (C::*)(Args...) const>
    :
    public func_args<R, this_pointer<C>, Args...>
  {
    static constexpr int num_args = sizeof...(Args) + 1;
  };

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
    :
    public func_traits<mem_functor<mem_func> >
  {
  private:
    mem_func m_held_func;

  public:
    typedef typename func_traits<mem_func>::retn_t R;

    mem_functor(mem_func f) : m_held_func(f) {}

    /// Function-call operator for object + any # args.
    template <class ptr, class... Args>
    R operator()(ptr obj, Args... args)
    {
      return ((*obj).*m_held_func)(args...);
    }
  };

  /// Factory function to make a mem_functor from any member function.
  template <class MF>
  inline mem_functor<MF> mem_func(MF mf)
  {
    return mem_functor<MF>(mf);
  }


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

  //  #######################################################
  //  =======================================================

  template <class base_functor, class bound_t>
  class bound_first;

  /// func_traits specialization for bound_first.
  template <class base_functor, class bound_t>
  struct func_traits<bound_first<base_functor, bound_t> >
  {
    static constexpr int num_args = func_traits<base_functor>::num_args-1;

    typedef typename func_traits<base_functor>::retn_t   retn_t;
    template <size_t N>
    struct arg
    {
      typedef typename func_traits<base_functor>::template arg<N+1>::type type;
    };
  };

  /// bound_first wraps another functor type with a fixed first argument.
  /** bound_first's can be constructed with the factory function
      bind_first(). */

  template <class base_functor, class bound_t>
  class bound_first
    :
    public func_traits<bound_first<base_functor, bound_t> >
  {
  private:
    base_functor m_held_func;
    bound_t m_bound;

  public:
    bound_first(base_functor base, bound_t bound) :
      m_held_func(base),
      m_bound(bound)
    {}

    bound_first(const bound_first& other) :
      m_held_func(other.m_held_func),
      m_bound(other.m_bound)
    {}

    typedef func_traits<bound_first<base_functor, bound_t> > traits;
    typedef typename traits::retn_t   retn_t;

    template <class... Args>
    retn_t operator()(Args... args)
    {
      return m_held_func(m_bound, args...);
    }

  private:
    bound_first& operator=(const bound_first&);
  };

  /// Factory function for creating bound_first functors.
  template <class base_functor, class bound_t>
  bound_first<base_functor, bound_t> bind_first(base_functor base,
                                                bound_t bound)
  {
    return bound_first<base_functor, bound_t>(base, bound);
  }


  //  #######################################################
  //  =======================================================

  template <class base_functor, class bound_t>
  class bound_last;

  /// func_traits specialization for bound_last.
  template <class base_functor, class bound_t>
  struct func_traits<bound_last<base_functor, bound_t> >
  {
    static constexpr int num_args = func_traits<base_functor>::num_args-1;

    typedef typename func_traits<base_functor>::retn_t   retn_t;
    template <size_t N>
    struct arg
    {
      typedef typename func_traits<base_functor>::template arg<N>::type type;
    };
  };

  /// bound_last wraps another functor type with a fixed last argument.
  /** bound_last objects can be constructed with the factory function
      bind_last(). */

  template <class base_functor, class bound_t>
  class bound_last
    :
    public func_traits<bound_last<base_functor, bound_t> >
  {
  private:
    base_functor m_held_func;
    bound_t m_bound;

  public:
    bound_last(base_functor base, bound_t bound) :
      m_held_func(base),
      m_bound(bound)
    {}

    bound_last(const bound_last& other) :
      m_held_func(other.m_held_func),
      m_bound(other.m_bound)
    {}

    typedef func_traits<bound_last<base_functor, bound_t> > traits;
    typedef typename traits::retn_t   retn_t;

    template <class... Args>
    retn_t operator()(Args... args)
    {
      return m_held_func(args..., m_bound);
    }

  private:
    bound_last& operator=(const bound_last&);
  };

  /// Factory function for creating bound_last functors.
  template <class base_functor, class bound_t>
  bound_last<base_functor, bound_t> bind_last(base_functor base,
                                              bound_t bound)
  {
    return bound_last<base_functor, bound_t>(base, bound);
  }

} // end namespace rutz

#endif // !GROOVX_RUTZ_FUNCTORS_H_UTC20050626084020_DEFINED
