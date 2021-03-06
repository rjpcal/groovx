/** @file tcl/makecmd.h construct tcl commands from c++ functions via
    templatized argument deduction and conversion between tcl and
    c++ */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Jun 22 09:07:27 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_TCL_MAKECMD_H_UTC20050628162421_DEFINED
#define GROOVX_TCL_MAKECMD_H_UTC20050628162421_DEFINED

#include "nub/ref.h"

#include "tcl/argspec.h"
#include "tcl/conversions.h"
#include "tcl/command.h"
#include "tcl/commandgroup.h"
#include "tcl/vecdispatch.h"

#include "rutz/functors.h"

#include <memory>

namespace rutz
{
  struct file_pos;
}

namespace tcl
{
  /// Specialization of help_convert for nub::ref.
  /** This allows us to pass+receive nub::ref objects from Tcl via the
      nub::uid's of the referred-to objects. */
  template <class T>
  struct help_convert<nub::ref<T> >
  {
    static nub::ref<T> from_tcl(Tcl_Obj* obj)
    {
      nub::uid uid = tcl::convert_to<nub::uid>(obj);
      return nub::ref<T>(uid);
    }
    static tcl::obj to_tcl(nub::ref<T> obj)
    {
      return convert_from(obj.id());
    }
  };

  /// Specialization of help_convert for nub::soft_ref.
  /** This allows us to pass+receive nub::soft_ref objects from Tcl via the
      nub::uid's of the referred-to objects. */
  template <class T>
  struct help_convert<nub::soft_ref<T> >
  {
    static nub::soft_ref<T> from_tcl(Tcl_Obj* obj)
    {
      nub::uid uid = tcl::convert_to<nub::uid>(obj);
      return nub::soft_ref<T>(uid);
    }
    static tcl::obj to_tcl(nub::soft_ref<T> obj)
    {
      return convert_from<nub::uid>(obj.id());
    }
  };

  template <class T>
  struct help_convert<rutz::this_pointer<T>>
  {
    static nub::soft_ref<T> from_tcl(Tcl_Obj* obj)
    {
      nub::uid uid = tcl::convert_to<nub::uid>(obj);
      return nub::soft_ref<T>(uid);
    }
  };

// ########################################################
/// Factory function for tcl::command's from void(tcl::call_context&) callables.

  template <class tcl_callable> // tcl_callable = callable void(tcl::call_context&)
  inline void
  make_generic_command(tcl::interpreter& interp,
                       tcl_callable f,
                       const char* cmd_name,
                       const char* usage,
                       const arg_spec& spec,
                       const rutz::file_pos& src_pos)
  {
    tcl::command_group::make(interp, f,
                             cmd_name, usage, spec, src_pos);
  }


// ########################################################
/// Factory function for vectorized tcl::command's from void(tcl::call_context&) callables.

  template <class tcl_callable> // tcl_callable = callable void(tcl::call_context&)
  inline void
  make_generic_vec_command(tcl::interpreter& interp,
                           tcl_callable f,
                           const char* cmd_name,
                           const char* usage,
                           const arg_spec& spec,
                           unsigned int keyarg,
                           const rutz::file_pos& src_pos)
  {
    tcl::command_group::make(interp, f,
                             cmd_name, usage, spec, src_pos,
                             tcl::get_vec_dispatcher(keyarg));
  }

///////////////////////////////////////////////////////////////////////
//
// Classes to transform callable with C++ params/return into void(tcl::call_context&)
//
///////////////////////////////////////////////////////////////////////

  template <size_t N, class R, class Func, class... DefaultArgs>
  class tcl_callable_base
  {
  private:
    Func m_held_func;
    std::tuple<std::decay_t<DefaultArgs>...> m_default_args;

    /// Extract parameter from given arguments
    template <size_t I>
    auto extract(tcl::call_context& ctx, std::true_type)
    {
      typedef typename rutz::func_traits<Func>::template arg<I>::type type;
      return ctx.template get_arg<type>(I+1);
    }

    /// Extract parameter from given arguments OR stored default args, depending on objc
    template <size_t I>
    auto extract(tcl::call_context& ctx, std::false_type)
    {
      if (I+1 >= ctx.objc())
        return std::get<I - (N - sizeof...(DefaultArgs))>(m_default_args);
      else
        return extract<I>(ctx, std::true_type());
    }

  protected:
    template <std::size_t... I>
    auto helper(tcl::call_context& ctx, std::index_sequence<I...>)
    {
      return m_held_func(extract<I>(ctx,
                                    std::conditional_t<(I < N - sizeof...(DefaultArgs)),
                                    std::true_type, std::false_type>())...);
    }

  public:
    tcl_callable_base(Func f, DefaultArgs&&... args)
      : m_held_func(f), m_default_args(args...)
    {}

    typedef R retn_t;
  };

  /// Generic tcl::tcl_callable definition.
  /** Takes a C++-style functor (could be a free function, or struct
      with operator()), and transforms it into a functor with an
      operator()(tcl::call_context&) which can be called from a
      tcl::command. This transformation requires extracting the
      appropriate parameters from the tcl::call_context, passing them
      to the C++ functor, and returning the result back to the
      tcl::call_context.
  */

  template <size_t N, class R, class Func, class... DefaultArgs>
  class tcl_callable : public tcl_callable_base<N,R,Func,DefaultArgs...>
  {
  public:
    tcl_callable(Func f, DefaultArgs&&... args)
      : tcl_callable_base<N,R,Func,DefaultArgs...>(f, std::forward<DefaultArgs>(args)...)
    {}

    void operator()(tcl::call_context& ctx)
    {
      R res(this->helper(ctx, std::make_index_sequence<N>()));
      ctx.set_result(std::move(res));
    }
  };

  template <size_t N, class Func, class... DefaultArgs>
  class tcl_callable<N,void,Func,DefaultArgs...> : public tcl_callable_base<N,void,Func,DefaultArgs...>
  {
  public:
    tcl_callable(Func f, DefaultArgs&&... args)
      : tcl_callable_base<N,void,Func,DefaultArgs...>(f, std::forward<DefaultArgs>(args)...)
    {}

    void operator()(tcl::call_context& ctx)
    {
      this->helper(ctx, std::make_index_sequence<N>());
    }
  };

// ########################################################
/// Factory function to make tcl::tcl_callable's from any functor or function ptr.

  template <class Fptr, class... DefaultArgs>
  inline auto
  build_tcl_callable(Fptr f, DefaultArgs&&... args)
  {
    return tcl_callable<rutz::func_traits<Fptr>::num_args,
                        typename rutz::func_traits<Fptr>::retn_t,
                        typename rutz::functor_of<Fptr>::type,
                        DefaultArgs...>
      (rutz::build_functor(f), std::forward<DefaultArgs>(args)...);
  }

///////////////////////////////////////////////////////////////////////
//
// And finally... make_command
//
///////////////////////////////////////////////////////////////////////


// ########################################################
/// Factory function for tcl::command's from function pointers.

  template <class Func, class... DefaultArgs>
  inline void
  make_command(tcl::interpreter& interp,
               Func f,
               const char* cmd_name,
               const char* usage,
               const rutz::file_pos& src_pos,
               DefaultArgs&&... args)
  {
    make_generic_command
      (interp, build_tcl_callable(f, std::forward<DefaultArgs>(args)...),
       cmd_name, usage,
       arg_spec(rutz::func_traits<Func>::num_args + 1 - sizeof...(DefaultArgs),
                rutz::func_traits<Func>::num_args + 1, false),
       src_pos);
  }

// ########################################################
/// Factory function for vectorized tcl::command's from function pointers.

  template <class Func, class... DefaultArgs>
  inline void
  make_vec_command(tcl::interpreter& interp,
                   Func f,
                   const char* cmd_name,
                   const char* usage,
                   unsigned int keyarg /*default is 1*/,
                   const rutz::file_pos& src_pos,
                   DefaultArgs&&... args)
  {
    make_generic_vec_command
      (interp, build_tcl_callable(f, std::forward<DefaultArgs>(args)...),
       cmd_name, usage,
       arg_spec(rutz::func_traits<Func>::num_args + 1 - sizeof...(DefaultArgs),
                rutz::func_traits<Func>::num_args + 1, false),
       keyarg, src_pos);
  }

} // end namespace tcl

#endif // !GROOVX_TCL_MAKECMD_H_UTC20050628162421_DEFINED
