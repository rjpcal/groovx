///////////////////////////////////////////////////////////////////////
//
// tclfunctor.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 22 09:07:27 2001
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

#ifndef TCLFUNCTOR_H_DEFINED
#define TCLFUNCTOR_H_DEFINED

#include "tcl/tclconvert.h"
#include "tcl/tclcmd.h"
#include "tcl/tclveccmd.h"

#include "util/functors.h"
#include "util/pointers.h"
#include "util/ref.h"

namespace Tcl
{
  /// Overload of fromTcl for Util::Ref.
  /** This allows us to receive Util::Ref objects from Tcl via the
      Util::UID's of the referred-to objects. */
  template <class T>
  inline Util::Ref<T> fromTcl(Tcl_Obj* obj, Util::Ref<T>*)
  {
    Util::UID uid = Tcl::toNative<Util::UID>(obj);
    return Util::Ref<T>(uid);
  }

  /// Overload of toTcl for Util::Ref.
  /** This allows us to pass Util::Ref objects to Tcl via the Util::UID's
      of the referred-to objects. */
  template <class T>
  inline Tcl::ObjPtr toTcl(Util::Ref<T> obj)
  {
    return toTcl(obj.id());
  }

  /// Overload of fromTcl for Util::SoftRef.
  /** This allows us to receive Util::SoftRef objects from Tcl via the
      Util::UID's of the referred-to objects. */
  template <class T>
  inline Util::SoftRef<T> fromTcl(Tcl_Obj* obj, Util::SoftRef<T>*)
  {
    Util::UID uid = Tcl::toNative<Util::UID>(obj);
    return Util::SoftRef<T>(uid);
  }

  /// Overload of toTcl for Util::SoftRef.
  /** This allows us to pass Util::SoftRef objects to Tcl via the
      Util::UID's of the referred-to objects. */
  template <class T>
  inline Tcl::ObjPtr toTcl(Util::SoftRef<T> obj)
  {
    return toTcl(obj.id());
  }


///////////////////////////////////////////////////////////////////////
//
// Functor<> template definitions. Each specialization takes a
// C++-style functor (could be a free function, or struct with
// operator()), and transforms it into a functor with an
// operator()(Tcl::Context&) which can be called from a
// Tcl::Command. This transformation requires extracting the
// appropriate parameters from the Tcl::Context, passing them to the
// C++ functor, and returning the result back to the Tcl::Context.
//
///////////////////////////////////////////////////////////////////////

#ifdef EXTRACT_PARAM
#  error EXTRACT_PARAM macro already defined
#endif

#define EXTRACT_PARAM(N) \
  typename Util::FuncTraits<Func>::Arg##N##_t p##N = \
  ctx.template getValFromArg<typename Util::FuncTraits<Func>::Arg##N##_t>(N);

  /// Generic Tcl::Functor definition.
  template <unsigned int N, class R, class Func>
  class Functor
  {};
}

namespace Util
{
  /// Specialization of FuncTraits for Tcl::Functor.
  template <unsigned int N, class F, class Func>
  struct FuncTraits<Tcl::Functor<N, F, Func> >
  {
    typedef typename Util::FuncTraits<Func>::Retn_t Retn_t;
  };
}

namespace Tcl
{


// ####################################################################
/// Tcl::Functor<0> -- zero arguments

  template <class R, class Func>
  struct Functor<0, R, Func>
  {
  private:
    Func itsHeldFunc;

  public:
    Functor<0, R, Func>(Func f) : itsHeldFunc(f) {}

    ~Functor() throw() {}

    R operator()(Tcl::Context& /*ctx*/)
    {
      return itsHeldFunc();
    }
  };


// ####################################################################
/// Tcl::Functor<1> -- one argument

  template <class R, class Func>
  struct Functor<1, R, Func>
  {
  private:
    Func itsHeldFunc;

  public:
    Functor<1, R, Func>(Func f) : itsHeldFunc(f) {}

    ~Functor() throw() {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1);
      return itsHeldFunc(p1);
    }
  };


// ####################################################################
/// Tcl::Functor<2> -- two arguments

  template <class R, class Func>
  struct Functor<2, R, Func>
  {
  private:
    Func itsHeldFunc;

  public:
    Functor<2, R, Func>(Func f) : itsHeldFunc(f) {}

    ~Functor() throw() {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2);
      return itsHeldFunc(p1, p2);
    }
  };


// ####################################################################
/// Tcl::Functor<3> -- three arguments

  template <class R, class Func>
  struct Functor<3, R, Func>
  {
  private:
    Func itsHeldFunc;

  public:
    Functor<3, R, Func>(Func f) : itsHeldFunc(f) {}

    ~Functor() throw() {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      return itsHeldFunc(p1, p2, p3);
    }
  };


// ####################################################################
/// Tcl::Functor<4> -- four arguments

  template <class R, class Func>
  struct Functor<4, R, Func>
  {
  private:
    Func itsHeldFunc;

  public:
    Functor<4, R, Func>(Func f) : itsHeldFunc(f) {}

    ~Functor() throw() {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      EXTRACT_PARAM(4);
      return itsHeldFunc(p1, p2, p3, p4);
    }
  };


// ####################################################################
/// Tcl::Functor<5> -- five arguments

  template <class R, class Func>
  struct Functor<5, R, Func>
  {
  private:
    Func itsHeldFunc;

  public:
    Functor<5, R, Func>(Func f) : itsHeldFunc(f) {}

    ~Functor() throw() {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      EXTRACT_PARAM(4); EXTRACT_PARAM(5);
      return itsHeldFunc(p1, p2, p3, p4, p5);
    }
  };


// ####################################################################
/// Tcl::Functor<6> -- six arguments

  template <class R, class Func>
  struct Functor<6, R, Func>
  {
  private:
    Func itsHeldFunc;

  public:
    Functor<6, R, Func>(Func f) : itsHeldFunc(f) {}

    ~Functor() throw() {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      EXTRACT_PARAM(4); EXTRACT_PARAM(5); EXTRACT_PARAM(6);
      return itsHeldFunc(p1, p2, p3, p4, p5, p6);
    }
  };

// ####################################################################
/// Tcl::Functor<7> -- seven arguments

  template <class R, class Func>
  struct Functor<7, R, Func>
  {
  private:
    Func itsHeldFunc;

  public:
    Functor<7, R, Func>(Func f) : itsHeldFunc(f) {}

    ~Functor() throw() {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      EXTRACT_PARAM(4); EXTRACT_PARAM(5); EXTRACT_PARAM(6);
      EXTRACT_PARAM(7);
      return itsHeldFunc(p1, p2, p3, p4, p5, p6, p7);
    }
  };

// ####################################################################
/// Tcl::Functor<8> -- eight arguments

  template <class R, class Func>
  struct Functor<8, R, Func>
  {
  private:
    Func itsHeldFunc;

  public:
    Functor<8, R, Func>(Func f) : itsHeldFunc(f) {}

    ~Functor() throw() {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      EXTRACT_PARAM(4); EXTRACT_PARAM(5); EXTRACT_PARAM(6);
      EXTRACT_PARAM(7); EXTRACT_PARAM(8);
      return itsHeldFunc(p1, p2, p3, p4, p5, p6, p7, p8);
    }
  };

#undef EXTRACT_PARAM

// ####################################################################
/// Factory function to make Tcl::Functor's from any functor or function ptr.

  template <class Fptr>
  inline Functor<Util::FuncTraits<Fptr>::numArgs,
                 typename Util::FuncTraits<Fptr>::Retn_t,
                 typename Util::FunctorOf<Fptr>::Type>
  buildTclFunctor(Fptr f)
  {
    return Util::buildFunctor(f);
  }


// ####################################################################
/// GenericCmd implements Tcl::Command using a held functor.

  template <class R, class Functor>
  class GenericCallback : public Callback
  {
  protected:
    GenericCallback<R, Functor>(Functor f) : itsHeldFunc(f) {}

  public:
    static shared_ptr<Callback> make(Functor f)
    {
      return shared_ptr<Callback>(new GenericCallback(f));
    }

    virtual ~GenericCallback() throw() {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      R res(itsHeldFunc(ctx)); ctx.setResult(res);
    }

  private:
    Functor itsHeldFunc;
  };

// ####################################################################
/// Specialization for functors with void return types.

  template <class Functor>
  class GenericCallback<void, Functor> : public Callback
  {
  protected:
    GenericCallback<void, Functor>(Functor f) : itsHeldFunc(f) {}

  public:
    static shared_ptr<Callback> make(Functor f)
    {
      return shared_ptr<Callback>(new GenericCallback(f));
    }

    virtual ~GenericCallback() throw() {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      itsHeldFunc(ctx);
    }

  private:
    Functor itsHeldFunc;
  };


// ####################################################################
/// Factory function for Tcl::Command's from functors.

  template <class Functor>
  inline shared_ptr<Command> makeGenericCmd
                               (Tcl::Interp& interp, Functor f,
                                const char* cmd_name,
                                const char* usage, int nargs,
                                const char* src_file_name,
                                int src_line_no)
  {
    typedef typename Util::FuncTraits<Functor>::Retn_t Retn_t;
    return Command::make(interp, GenericCallback<Retn_t, Functor>::make(f),
                         cmd_name, usage, nargs+1,
                         -1 /*default for objc_max*/,
                         false /*default for exact_objc*/,
                         src_file_name,
                         src_line_no);
  }


// ####################################################################
/// Factory function for vectorized Tcl::Command's from functors.

  template <class Functor>
  inline shared_ptr<Command> makeGenericVecCmd
                               (Tcl::Interp& interp, Functor f,
                                const char* cmd_name, const char* usage,
                                int nargs, unsigned int keyarg,
                                const char* src_file_name,
                                int src_line_no)
  {
    typedef typename Util::FuncTraits<Functor>::Retn_t Retn_t;
    shared_ptr<Command> cmd =
      Command::make(interp, GenericCallback<Retn_t, Functor>::make(f),
                    cmd_name, usage, nargs+1,
                    -1 /*default for objc_max*/,
                    false /*default for exact_objc*/,
                    src_file_name,
                    src_line_no);
    Tcl::useVecDispatch(*cmd, keyarg);
    return cmd;
  }

///////////////////////////////////////////////////////////////////////
//
// And finally... makeCmd
//
///////////////////////////////////////////////////////////////////////

// ####################################################################
/// Factory function for Tcl::Command's from function pointers.

  template <class Func>
  inline shared_ptr<Command> makeCmd
                              (Tcl::Interp& interp, Func f,
                               const char* cmd_name, const char* usage,
                               const char* src_file_name,
                               int src_line_no)
  {
    return makeGenericCmd
      (interp, buildTclFunctor(f), cmd_name, usage,
       Util::FuncTraits<Func>::numArgs,
       src_file_name, src_line_no);
  }

// ####################################################################
/// Factory function for vectorized Tcl::Command's from function pointers.

  template <class Func>
  inline shared_ptr<Command> makeVecCmd
                              (Tcl::Interp& interp, Func f,
                               const char* cmd_name, const char* usage,
                               unsigned int keyarg /*default is 1*/,
                               const char* src_file_name,
                               int src_line_no)
  {
    return makeGenericVecCmd
      (interp, buildTclFunctor(f), cmd_name, usage,
       Util::FuncTraits<Func>::numArgs, keyarg,
       src_file_name, src_line_no);
  }

} // end namespace Tcl

static const char vcid_tclfunctor_h[] = "$Header$";
#endif // !TCLFUNCTOR_H_DEFINED
