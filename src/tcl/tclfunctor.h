///////////////////////////////////////////////////////////////////////
//
// tclfunctor.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 22 09:07:27 2001
// written: Fri Jan 18 16:06:56 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLFUNCTOR_H_DEFINED
#define TCLFUNCTOR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCONVERT_H_DEFINED)
#include "tcl/tclconvert.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCMD_H_DEFINED)
#include "tcl/tclcmd.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLVECCMD_H_DEFINED)
#include "tcl/tclveccmd.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FUNCTORS_H_DEFINED)
#include "util/functors.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REF_H_DEFINED)
#include "util/ref.h"
#endif

namespace Tcl
{

///////////////////////////////////////////////////////////////////////
//
// Convert<> specialization allows us to pass Ref<>'s and SoftRef<>'s
// to and from Tcl
//
///////////////////////////////////////////////////////////////////////

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

  template <unsigned int N, class R, class Func>
  class Functor
  {};
}

namespace Util
{
  template <unsigned int N, class F, class Func>
  struct FuncTraits<Tcl::Functor<N, F, Func> >
  {
    typedef typename Util::FuncTraits<Func>::Retn_t Retn_t;
  };
}

namespace Tcl
{

  template <class Func>
  struct FunctorBase : protected Util::FuncHolder<Func>
  {
    FunctorBase<Func>(Func f) : Util::FuncHolder<Func>(f) {}
  };


///////////////////////////////////////////////////////////////////////
//
// zero arguments -- Functor<0>
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  struct Functor<0, R, Func> : public FunctorBase<Func>
  {
    Functor<0, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      return (itsHeldFunc());
    }
  };


///////////////////////////////////////////////////////////////////////
//
// one argument -- Functor<1>
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  struct Functor<1, R, Func> : public FunctorBase<Func>
  {
    Functor<1, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1);
      return (itsHeldFunc(p1));
    }
  };


///////////////////////////////////////////////////////////////////////
//
// two arguments -- Functor<2>
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  struct Functor<2, R, Func> : public FunctorBase<Func>
  {
    Functor<2, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2);
      return (itsHeldFunc(p1, p2));
    }
  };


///////////////////////////////////////////////////////////////////////
//
// three arguments -- Functor<3>
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  struct Functor<3, R, Func> : public FunctorBase<Func>
  {
    Functor<3, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      return (itsHeldFunc(p1, p2, p3));
    }
  };


///////////////////////////////////////////////////////////////////////
//
// four arguments -- Functor<4>
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  struct Functor<4, R, Func> : public FunctorBase<Func>
  {
    Functor<4, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      EXTRACT_PARAM(4);
      return (itsHeldFunc(p1, p2, p3, p4));
    }
  };


///////////////////////////////////////////////////////////////////////
//
// five arguments -- Functor<5>
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  struct Functor<5, R, Func> : public FunctorBase<Func>
  {
    Functor<5, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      EXTRACT_PARAM(4); EXTRACT_PARAM(5);
      return (itsHeldFunc(p1, p2, p3, p4, p5));
    }
  };


///////////////////////////////////////////////////////////////////////
//
// six arguments -- Functor<6>
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  struct Functor<6, R, Func> : public FunctorBase<Func>
  {
    Functor<6, R, Func>(Func f) : FunctorBase<Func>(f) {}

    R operator()(Tcl::Context& ctx)
    {
      EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      EXTRACT_PARAM(4); EXTRACT_PARAM(5); EXTRACT_PARAM(6);
      return (itsHeldFunc(p1, p2, p3, p4, p5, p6));
    }
  };

#undef EXTRACT_PARAM

///////////////////////////////////////////////////////////////////////
//
// buildTclFunctor: a "generic constructor" for free function pointers.
//
///////////////////////////////////////////////////////////////////////

  template <class Fptr>
  inline Functor<Util::FuncTraits<Fptr>::numArgs,
                 typename Util::FuncTraits<Fptr>::Retn_t,
                 Fptr>
  buildTclFunctor(Fptr f)
  {
    return f;
  }


///////////////////////////////////////////////////////////////////////
//
// buildTclFunctor specializations for member functions
//
///////////////////////////////////////////////////////////////////////

  template <class R, class C>
  inline Functor<1, R, Util::MemFunctor<R (C::*)()> >
  buildTclFunctor(R (C::*mf)())
  {
    return Util::MemFunctor<R (C::*)()>(mf);
  }

  template <class R, class C, class P1>
  inline Functor<2, R, Util::MemFunctor<R (C::*)(P1)> >
  buildTclFunctor(R (C::*mf)(P1))
  {
    return Util::MemFunctor<R (C::*)(P1)>(mf);
  }

  template <class R, class C, class P1, class P2>
  inline Functor<3, R, Util::MemFunctor<R (C::*)(P1, P2)> >
  buildTclFunctor(R (C::*mf)(P1,P2))
  {
    return Util::MemFunctor<R (C::*)(P1, P2)>(mf);
  }

  template <class R, class C, class P1, class P2, class P3>
  inline Functor<4, R, Util::MemFunctor<R (C::*)(P1, P2, P3)> >
  buildTclFunctor(R (C::*mf)(P1,P2,P3))
  {
    return Util::MemFunctor<R (C::*)(P1, P2, P3)>(mf);
  }

  template <class R, class C, class P1, class P2, class P3, class P4>
  inline Functor<5, R, Util::MemFunctor<R (C::*)(P1, P2, P3, P4)> >
  buildTclFunctor(R (C::*mf)(P1,P2,P3,P4))
  {
    return Util::MemFunctor<R (C::*)(P1, P2, P3, P4)>(mf);
  }


///////////////////////////////////////////////////////////////////////
//
// GenericCmd
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Functor>
  class GenericCmd : public Command, private Util::FuncHolder<Functor>
  {
  public:
    GenericCmd<R, Functor>(Tcl_Interp* interp, Functor f, const char* cmd_name,
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

  template <class Functor>
  class GenericCmd<void, Functor> : public Command,
                                    private Util::FuncHolder<Functor>
  {
  public:
    GenericCmd<void, Functor>(Tcl_Interp* interp, Functor f,
                              const char* cmd_name, const char* usage,
                              int nargs) :
      Command(interp, cmd_name, usage, nargs+1),
      Util::FuncHolder<Functor>(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      itsHeldFunc(ctx);
    }
  };


  template <class Functor>
  inline Command* makeGenericCmd(Tcl_Interp* interp, Functor f,
                                 const char* cmd_name, const char* usage,
                                 int nargs)
  {
    return new GenericCmd<typename Util::FuncTraits<Functor>::Retn_t, Functor>
      (interp, f, cmd_name, usage, nargs);
  }


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

  template <class Func>
  inline Command* makeCmd(Tcl_Interp* interp, Func f,
                          const char* cmd_name, const char* usage)
  {
    return makeGenericCmd(interp, buildTclFunctor(f), cmd_name, usage,
                          Util::FuncTraits<Func>::numArgs);
  }

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
