///////////////////////////////////////////////////////////////////////
//
// tclfunctor.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 22 09:07:27 2001
// written: Wed Sep 25 18:57:04 2002
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

  /// Specialization of Convert for Util::Ref.
  /** This allows us to pass Util::Ref objects to and from Tcl via the
      Util::UID's of the referred-to objects. */
  template <class T>
  struct Convert<Ref<T> >
  {
    static Ref<T> fromTcl(Tcl_Obj* obj)
    {
      Util::UID uid = Convert<Util::UID>::fromTcl(obj);
      return Ref<T>(uid);
    }

    static Tcl::ObjPtr toTcl(Ref<T> obj)
    {
      return Convert<Util::UID>::toTcl(obj.id());
    }
  };

  /// Specialization of Convert for Util::SoftRef.
  /** This allows us to pass Util::SoftRef objects to and from Tcl via the
      Util::UID's of the referred-to objects. */
  template <class T>
  struct Convert<SoftRef<T> >
  {
    static SoftRef<T> fromTcl(Tcl_Obj* obj)
    {
      Util::UID uid = Convert<Util::UID>::fromTcl(obj);
      return SoftRef<T>(uid);
    }

    static Tcl::ObjPtr toTcl(SoftRef<T> obj)
    {
      return Convert<Util::UID>::toTcl(obj.id());
    }
  };


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
  ctx.getValFromArg(N, TypeCue<typename Util::FuncTraits<Func>::Arg##N##_t>());

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
/// A factory function for making Tcl::Functor's from free functions.

  template <class Fptr>
  inline Functor<Util::FuncTraits<Fptr>::numArgs,
                 typename Util::FuncTraits<Fptr>::Retn_t,
                 Fptr>
  buildTclFunctor(Fptr f)
  {
    return f;
  }


// ####################################################################
/// Factory function for making Tcl::Functor's from 0-arg member functions.

  template <class R, class C>
  inline Functor<1, R, Util::MemFunctor<R (C::*)()> >
  buildTclFunctor(R (C::*mf)())
  {
    return Util::MemFunctor<R (C::*)()>(mf);
  }

// ####################################################################
/// Factory function for making Tcl::Functor's from 0-arg member functions.

  template <class R, class C>
  inline Functor<1, R, Util::MemFunctor<R (C::*)() const> >
  buildTclFunctor(R (C::*mf)() const)
  {
    return Util::MemFunctor<R (C::*)() const>(mf);
  }

// ####################################################################
/// Factory function for making Tcl::Functor's from 1-arg member functions.

  template <class R, class C, class P1>
  inline Functor<2, R, Util::MemFunctor<R (C::*)(P1)> >
  buildTclFunctor(R (C::*mf)(P1))
  {
    return Util::MemFunctor<R (C::*)(P1)>(mf);
  }

// ####################################################################
/// Factory function for making Tcl::Functor's from 1-arg member functions.

  template <class R, class C, class P1>
  inline Functor<2, R, Util::MemFunctor<R (C::*)(P1) const> >
  buildTclFunctor(R (C::*mf)(P1) const)
  {
    return Util::MemFunctor<R (C::*)(P1) const>(mf);
  }

// ####################################################################
/// Factory function for making Tcl::Functor's from 2-arg member functions.

  template <class R, class C, class P1, class P2>
  inline Functor<3, R, Util::MemFunctor<R (C::*)(P1, P2)> >
  buildTclFunctor(R (C::*mf)(P1,P2))
  {
    return Util::MemFunctor<R (C::*)(P1, P2)>(mf);
  }

// ####################################################################
/// Factory function for making Tcl::Functor's from 2-arg member functions.

  template <class R, class C, class P1, class P2>
  inline Functor<3, R, Util::MemFunctor<R (C::*)(P1, P2) const> >
  buildTclFunctor(R (C::*mf)(P1,P2) const)
  {
    return Util::MemFunctor<R (C::*)(P1, P2) const>(mf);
  }

// ####################################################################
/// Factory function for making Tcl::Functor's from 3-arg member functions.

  template <class R, class C, class P1, class P2, class P3>
  inline Functor<4, R, Util::MemFunctor<R (C::*)(P1, P2, P3)> >
  buildTclFunctor(R (C::*mf)(P1,P2,P3))
  {
    return Util::MemFunctor<R (C::*)(P1, P2, P3)>(mf);
  }

// ####################################################################
/// Factory function for making Tcl::Functor's from 3-arg member functions.

  template <class R, class C, class P1, class P2, class P3>
  inline Functor<4, R, Util::MemFunctor<R (C::*)(P1, P2, P3) const> >
  buildTclFunctor(R (C::*mf)(P1,P2,P3) const)
  {
    return Util::MemFunctor<R (C::*)(P1, P2, P3) const>(mf);
  }

// ####################################################################
/// Factory function for making Tcl::Functor's from 4-arg member functions.

  template <class R, class C, class P1, class P2, class P3, class P4>
  inline Functor<5, R, Util::MemFunctor<R (C::*)(P1, P2, P3, P4)> >
  buildTclFunctor(R (C::*mf)(P1,P2,P3,P4))
  {
    return Util::MemFunctor<R (C::*)(P1, P2, P3, P4)>(mf);
  }

// ####################################################################
/// Factory function for making Tcl::Functor's from 4-arg member functions.

  template <class R, class C, class P1, class P2, class P3, class P4>
  inline Functor<5, R, Util::MemFunctor<R (C::*)(P1, P2, P3, P4) const> >
  buildTclFunctor(R (C::*mf)(P1,P2,P3,P4) const)
  {
    return Util::MemFunctor<R (C::*)(P1, P2, P3, P4) const>(mf);
  }



// ####################################################################
/// GenericCmd implements Tcl::Command using a held functor.

  template <class R, class Functor>
  class GenericCmd : public Command, private Util::FuncHolder<Functor>
  {
  public:
    GenericCmd<R, Functor>(Tcl_Interp* interp, Functor f,
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
    GenericCmd<void, Functor>(Tcl_Interp* interp, Functor f,
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
  inline Command* makeGenericCmd(Tcl_Interp* interp, Functor f,
                                 const char* cmd_name,
                                 const char* usage, int nargs)
  {
    return new GenericCmd<typename Util::FuncTraits<Functor>::Retn_t, Functor>
      (interp, f, cmd_name, usage, nargs);
  }


// ####################################################################
/// Factory function for vectorized Tcl::Command's from functors.

  template <class Functor>
  inline Command* makeGenericVecCmd(Tcl_Interp* interp, Functor f,
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
  inline Command* makeCmd(Tcl_Interp* interp, Func f,
                          const char* cmd_name, const char* usage)
  {
    return makeGenericCmd(interp, buildTclFunctor(f), cmd_name, usage,
                          Util::FuncTraits<Func>::numArgs);
  }

// ####################################################################
/// Factory function for vectorized Tcl::Command's from function pointers.

  template <class Func>
  inline Command* makeVecCmd(Tcl_Interp* interp, Func f,
                             const char* cmd_name, const char* usage,
                             unsigned int keyarg=1)
  {
    return makeGenericVecCmd(interp, buildTclFunctor(f), cmd_name, usage,
                             Util::FuncTraits<Func>::numArgs, keyarg);
  }

} // end namespace Tcl

static const char vcid_tclfunctor_h[] = "$Header$";
#endif // !TCLFUNCTOR_H_DEFINED
