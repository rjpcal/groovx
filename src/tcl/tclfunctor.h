///////////////////////////////////////////////////////////////////////
//
// tclfunctor.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 22 09:07:27 2001
// written: Tue Dec 10 16:57:54 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLFUNCTOR_H_DEFINED
#define TCLFUNCTOR_H_DEFINED

#include "tcl/tclconvert.h"
#include "tcl/tclcmd.h"
#include "tcl/tclveccmd.h"

#include "util/functors.h"
#include "util/ref.h"

namespace Tcl
{
  /// Overload of fromTcl for Util::Ref.
  /** This allows us to receive Util::Ref objects from Tcl via the
      Util::UID's of the referred-to objects. */
  template <class T>
  inline Ref<T> fromTcl(Tcl_Obj* obj, Ref<T>*)
  {
    Util::UID uid = Tcl::toNative<Util::UID>(obj);
    return Ref<T>(uid);
  }

  /// Overload of toTcl for Util::Ref.
  /** This allows us to pass Util::Ref objects to Tcl via the Util::UID's
      of the referred-to objects. */
  template <class T>
  inline Tcl::ObjPtr toTcl(Ref<T> obj)
  {
    return toTcl(obj.id());
  }

  /// Overload of fromTcl for Util::SoftRef.
  /** This allows us to receive Util::SoftRef objects from Tcl via the
      Util::UID's of the referred-to objects. */
  template <class T>
  inline SoftRef<T> fromTcl(Tcl_Obj* obj, SoftRef<T>*)
  {
    Util::UID uid = Tcl::toNative<Util::UID>(obj);
    return SoftRef<T>(uid);
  }

  /// Overload of toTcl for Util::SoftRef.
  /** This allows us to pass Util::SoftRef objects to Tcl via the
      Util::UID's of the referred-to objects. */
  template <class T>
  inline Tcl::ObjPtr toTcl(SoftRef<T> obj)
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
/// Tcl::FunctorBase

  template <class Func>
  struct FunctorBase : protected Util::FuncHolder<Func>
  {
    FunctorBase<Func>(Func f) : Util::FuncHolder<Func>(f) {}
  };


// ####################################################################
/// Tcl::Functor<0> -- zero arguments

  template <class R, class Func>
  struct Functor<0, R, Func> : public FunctorBase<Func>
  {
  public:
    Functor<0, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& /*ctx*/)
    {
      return (itsHeldFunc());
    }
  };


// ####################################################################
/// Tcl::Functor<1> -- one argument

  template <class R, class Func>
  struct Functor<1, R, Func> : public FunctorBase<Func>
  {
  public:
    Functor<1, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1);
      return (itsHeldFunc(p1));
    }
  };


// ####################################################################
/// Tcl::Functor<2> -- two arguments

  template <class R, class Func>
  struct Functor<2, R, Func> : public FunctorBase<Func>
  {
  public:
    Functor<2, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2);
      return (itsHeldFunc(p1, p2));
    }
  };


// ####################################################################
/// Tcl::Functor<3> -- three arguments

  template <class R, class Func>
  struct Functor<3, R, Func> : public FunctorBase<Func>
  {
  public:
    Functor<3, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      return (itsHeldFunc(p1, p2, p3));
    }
  };


// ####################################################################
/// Tcl::Functor<4> -- four arguments

  template <class R, class Func>
  struct Functor<4, R, Func> : public FunctorBase<Func>
  {
  public:
    Functor<4, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      EXTRACT_PARAM(4);
      return (itsHeldFunc(p1, p2, p3, p4));
    }
  };


// ####################################################################
/// Tcl::Functor<5> -- five arguments

  template <class R, class Func>
  struct Functor<5, R, Func> : public FunctorBase<Func>
  {
  public:
    Functor<5, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      EXTRACT_PARAM(4); EXTRACT_PARAM(5);
      return (itsHeldFunc(p1, p2, p3, p4, p5));
    }
  };


// ####################################################################
/// Tcl::Functor<6> -- six arguments

  template <class R, class Func>
  struct Functor<6, R, Func> : public FunctorBase<Func>
  {
  public:
    Functor<6, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      EXTRACT_PARAM(4); EXTRACT_PARAM(5); EXTRACT_PARAM(6);
      return (itsHeldFunc(p1, p2, p3, p4, p5, p6));
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
  class GenericCmd : public Command, private Util::FuncHolder<Functor>
  {
  public:
    GenericCmd<R, Functor>(Tcl::Interp& interp, Functor f,
                           const char* cmd_name,
                           const char* usage, int nargs) :
      Command(interp, cmd_name, usage, nargs+1),
      Util::FuncHolder<Functor>(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      R res(itsHeldFunc(ctx)); ctx.setResult(res);
    }
  };

// ####################################################################
/// Specialization for functors with void return types.

  template <class Functor>
  class GenericCmd<void, Functor> : public Command,
                                    private Util::FuncHolder<Functor>
  {
  public:
    GenericCmd<void, Functor>(Tcl::Interp& interp, Functor f,
                              const char* cmd_name,
                              const char* usage, int nargs) :
      Command(interp, cmd_name, usage, nargs+1),
      Util::FuncHolder<Functor>(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      itsHeldFunc(ctx);
    }
  };


// ####################################################################
/// Factory function for Tcl::Command's from functors.

  template <class Functor>
  inline Command* makeGenericCmd(Tcl::Interp& interp, Functor f,
                                 const char* cmd_name,
                                 const char* usage, int nargs)
  {
    return new GenericCmd<typename Util::FuncTraits<Functor>::Retn_t, Functor>
      (interp, f, cmd_name, usage, nargs);
  }


// ####################################################################
/// Factory function for vectorized Tcl::Command's from functors.

  template <class Functor>
  inline Command* makeGenericVecCmd(Tcl::Interp& interp, Functor f,
                                    const char* cmd_name, const char* usage,
                                    int nargs, unsigned int keyarg)
  {
    Command* cmd =
      new GenericCmd<typename Util::FuncTraits<Functor>::Retn_t, Functor>
      (interp, f, cmd_name, usage, nargs);
    Tcl::useVecDispatch(cmd, keyarg);
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
  inline Command* makeCmd(Tcl::Interp& interp, Func f,
                          const char* cmd_name, const char* usage)
  {
    return makeGenericCmd(interp, buildTclFunctor(f), cmd_name, usage,
                          Util::FuncTraits<Func>::numArgs);
  }

// ####################################################################
/// Factory function for vectorized Tcl::Command's from function pointers.

  template <class Func>
  inline Command* makeVecCmd(Tcl::Interp& interp, Func f,
                             const char* cmd_name, const char* usage,
                             unsigned int keyarg=1)
  {
    return makeGenericVecCmd(interp, buildTclFunctor(f), cmd_name, usage,
                             Util::FuncTraits<Func>::numArgs, keyarg);
  }

} // end namespace Tcl

static const char vcid_tclfunctor_h[] = "$Header$";
#endif // !TCLFUNCTOR_H_DEFINED
