///////////////////////////////////////////////////////////////////////
//
// functor.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 22 09:07:27 2001
// written: Fri Jul 13 15:27:59 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FUNCTOR_H_DEFINED
#define FUNCTOR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLVECCMD_H_DEFINED)
#include "tcl/tclveccmd.h"
#endif

namespace Tcl
{

///////////////////////////////////////////////////////////////////////
//
// FuncTraits
//
///////////////////////////////////////////////////////////////////////

  template <class Func>
  struct FuncTraits {};

  template <class R>
  struct FuncTraits<R (*)()>
  {
    enum { numArgs = 0 };
  };

  template <class R, class P1>
  struct FuncTraits<R (*)(P1)>
  {
    enum { numArgs = 1 };
    typedef P1 Arg1_t;
  };

  template <class R, class P1, class P2>
  struct FuncTraits<R (*)(P1, P2)>
  {
    enum { numArgs = 2 };
    typedef P1 Arg1_t;
    typedef P2 Arg2_t;
  };

  template <class R, class P1, class P2, class P3>
  struct FuncTraits<R (*)(P1, P2, P3)>
  {
    enum { numArgs = 3 };
    typedef P1 Arg1_t;
    typedef P2 Arg2_t;
    typedef P3 Arg3_t;
  };

  template <class R, class P1, class P2, class P3, class P4>
  struct FuncTraits<R (*)(P1, P2, P3, P4)>
  {
    enum { numArgs = 4 };
    typedef P1 Arg1_t;
    typedef P2 Arg2_t;
    typedef P3 Arg3_t;
    typedef P4 Arg4_t;
  };

  template <class R, class P1, class P2, class P3, class P4, class P5>
  struct FuncTraits<R (*)(P1, P2, P3, P4, P5)>
  {
    enum { numArgs = 5 };
    typedef P1 Arg1_t;
    typedef P2 Arg2_t;
    typedef P3 Arg3_t;
    typedef P4 Arg4_t;
    typedef P5 Arg5_t;
  };

  template <class R, class P1, class P2, class P3, class P4, class P5, class P6>
  struct FuncTraits<R (*)(P1, P2, P3, P4, P5, P6)>
  {
    enum { numArgs = 6 };
    typedef P1 Arg1_t;
    typedef P2 Arg2_t;
    typedef P3 Arg3_t;
    typedef P4 Arg4_t;
    typedef P5 Arg5_t;
    typedef P6 Arg6_t;
  };

  //
  // This macro avoids some of the repetitive typing...
  //

#ifdef EXTRACT_PARAM
#  error EXTRACT_PARAM macro already defined
#endif

#define EXTRACT_PARAM(N) \
  typename FuncTraits<Func>::Arg##N##_t p##N = \
  ctx.getValFromArg(N, TypeCue<typename FuncTraits<Func>::Arg##N##_t>());

///////////////////////////////////////////////////////////////////////
//
// zero arguments -- Functor0
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  class Functor0 {
  public:
    Functor0<R, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
      ctx.setResult(itsFunc());
    }

  private:
    Func itsFunc;
  };

  template <class Func>
  class Functor0<void, Func> {
  public:
    Functor0<void, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
      itsFunc();
    }

  private:
    Func itsFunc;
  };


///////////////////////////////////////////////////////////////////////
//
// one argument -- Functor1
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  class Functor1 {
  public:
    Functor1<R, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1);
      ctx.setResult(itsFunc(p1));
    }

  private:
    Func itsFunc;
  };

  template <class Func>
  class Functor1<void, Func> {
  public:
    Functor1<void, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1);
      itsFunc(p1);
    }

  private:
    Func itsFunc;
  };


///////////////////////////////////////////////////////////////////////
//
// two arguments -- Functor2
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  class Functor2 {
  public:
    Functor2<R, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1); EXTRACT_PARAM(2);
      ctx.setResult(itsFunc(p1, p2));
    }

  private:
    Func itsFunc;
  };

  template <class Func>
  class Functor2<void, Func> {
  public:
    Functor2<void, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1); EXTRACT_PARAM(2);
      itsFunc(p1, p2);
    }

  private:
    Func itsFunc;
  };


///////////////////////////////////////////////////////////////////////
//
// three arguments -- Functor3
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  class Functor3 {
  public:
    Functor3<R, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      ctx.setResult(itsFunc(p1, p2, p3));
    }

  private:
    Func itsFunc;
  };

  template <class Func>
  class Functor3<void, Func> {
  public:
    Functor3<void, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
      itsFunc(p1, p2, p3);
    }

  private:
    Func itsFunc;
  };


///////////////////////////////////////////////////////////////////////
//
// four arguments -- Functor4
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  class Functor4 {
  public:
    Functor4<R, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
		EXTRACT_PARAM(4);
      ctx.setResult(itsFunc(p1, p2, p3, p4));
    }

  private:
    Func itsFunc;
  };

  template <class Func>
  class Functor4<void, Func> {
  public:
    Functor4<void, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
		EXTRACT_PARAM(4);
      itsFunc(p1, p2, p3, p4);
    }

  private:
    Func itsFunc;
  };


///////////////////////////////////////////////////////////////////////
//
// five arguments -- Functor5
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  class Functor5 {
  public:
    Functor5<R, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
		EXTRACT_PARAM(4); EXTRACT_PARAM(5);
      ctx.setResult(itsFunc(p1, p2, p3, p4, p5));
    }

  private:
    Func itsFunc;
  };

  template <class Func>
  class Functor5<void, Func> {
  public:
    Functor5<void, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
		EXTRACT_PARAM(4); EXTRACT_PARAM(5);
      itsFunc(p1, p2, p3, p4, p5);
    }

  private:
    Func itsFunc;
  };


///////////////////////////////////////////////////////////////////////
//
// six arguments -- Functor6
//
///////////////////////////////////////////////////////////////////////

  template <class R, class Func>
  class Functor6 {
  public:
    Functor6<R, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
		EXTRACT_PARAM(4); EXTRACT_PARAM(5); EXTRACT_PARAM(6);
      ctx.setResult(itsFunc(p1, p2, p3, p4, p5, p6));
    }

  private:
    Func itsFunc;
  };

  template <class Func>
  class Functor6<void, Func> {
  public:
    Functor6<void, Func>(Func f) : itsFunc(f) {}

    void operator()(Tcl::Context& ctx)
    {
		EXTRACT_PARAM(1); EXTRACT_PARAM(2); EXTRACT_PARAM(3);
		EXTRACT_PARAM(4); EXTRACT_PARAM(5); EXTRACT_PARAM(6);
      itsFunc(p1, p2, p3, p4, p5, p6);
    }

  private:
    Func itsFunc;
  };

#undef EXTRACT_PARAM

///////////////////////////////////////////////////////////////////////
//
// Functor "generic constructors" for free functions
//
///////////////////////////////////////////////////////////////////////

  template <class R>
  inline Functor0<R, R (*)()>
  wrapFunc(R (*f)())
  {
    return Functor0<R, R (*)()>(f);
  }

  template <class R, class P1>
  inline Functor1<R, R (*)(P1)>
  wrapFunc(R (*f)(P1))
  {
    return Functor1<R, R (*)(P1)>(f);
  }

  template <class R, class P1, class P2>
  inline Functor2<R, R (*)(P1, P2)>
  wrapFunc(R (*f)(P1, P2))
  {
    return Functor2<R, R (*)(P1, P2)>(f);
  }

  template <class R, class P1, class P2, class P3>
  inline Functor3<R, R (*)(P1, P2, P3)>
  wrapFunc(R (*f)(P1, P2, P3))
  {
    return Functor3<R, R (*)(P1, P2, P3)>(f);
  }

  template <class R, class P1, class P2, class P3, class P4>
  inline Functor4<R, R (*)(P1, P2, P3, P4)>
  wrapFunc(R (*f)(P1, P2, P3, P4))
  {
    return Functor4<R, R (*)(P1, P2, P3, P4)>(f);
  }

  template <class R, class P1, class P2, class P3, class P4, class P5>
  inline Functor5<R, R (*)(P1, P2, P3, P4, P5)>
  wrapFunc(R (*f)(P1, P2, P3, P4, P5))
  {
    return Functor5<R, R (*)(P1, P2, P3, P4, P5)>(f);
  }

  template <class R, class P1, class P2, class P3, class P4, class P5, class P6>
  inline Functor6<R, R (*)(P1, P2, P3, P4, P5, P6)>
  wrapFunc(R (*f)(P1, P2, P3, P4, P5, P6))
  {
    return Functor6<R, R (*)(P1, P2, P3, P4, P5, P6)>(f);
  }


///////////////////////////////////////////////////////////////////////
//
// GenericCmd
//
///////////////////////////////////////////////////////////////////////

  template <class Functor>
  class GenericCmd : public TclCmd {
  public:

    GenericCmd<Functor>(Tcl_Interp* interp, Functor f, const char* cmd_name,
                        const char* usage, int nargs) :
      TclCmd(interp, cmd_name, usage, nargs+1),
      itsFunctor(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      itsFunctor(ctx);
    }

  private:
    Functor itsFunctor;
  };


  template <class Functor>
  inline TclCmd* makeGenericCmd(Tcl_Interp* interp, Functor f,
                                const char* cmd_name, const char* usage,
                                int nargs)
  {
    return new GenericCmd<Functor>(interp, f, cmd_name, usage, nargs);
  }


///////////////////////////////////////////////////////////////////////
//
// GenericVecCmd
//
///////////////////////////////////////////////////////////////////////

  template <class Functor>
  class GenericVecCmd : public VecCmd {
  public:

    GenericVecCmd<Functor>(Tcl_Interp* interp, Functor f, const char* cmd_name,
                           const char* usage, int nargs, unsigned int keyarg)
      :
      VecCmd(interp, cmd_name, usage, keyarg, nargs+1),
      itsFunctor(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      itsFunctor(ctx);
    }

  private:
    Functor itsFunctor;
  };


  template <class Functor>
  inline TclCmd* makeGenericVecCmd(Tcl_Interp* interp, Functor f,
                                   const char* cmd_name, const char* usage,
                                   int nargs, unsigned int keyarg)
  {
    return new GenericVecCmd<Functor>(interp, f, cmd_name,
                                      usage, nargs, keyarg);
  }

///////////////////////////////////////////////////////////////////////
//
// And finally... makeCmd
//
///////////////////////////////////////////////////////////////////////

  template <class Func>
  inline TclCmd* makeCmd(Tcl_Interp* interp, Func f,
                         const char* cmd_name, const char* usage)
  {
    return makeGenericCmd(interp, wrapFunc(f), cmd_name, usage,
                          FuncTraits<Func>::numArgs);
  }

  template <class Func>
  inline TclCmd* makeVecCmd(Tcl_Interp* interp, Func f,
                            const char* cmd_name, const char* usage,
                            unsigned int keyarg=1)
  {
    return makeGenericVecCmd(interp, wrapFunc(f), cmd_name, usage,
                             FuncTraits<Func>::numArgs, keyarg);
  }

///////////////////////////////////////////////////////////////////////
//
// versions that can be used with TclPkg's
//
///////////////////////////////////////////////////////////////////////

  template <class Pkg, class Func>
  inline void def(Pkg* pkg, Func f, const char* cmd_name, const char* usage)
  {
    pkg->addCommand( makeCmd(pkg->interp(), f, cmd_name, usage) );
  }

  template <class Pkg, class Func>
  inline void defVec(Pkg* pkg, Func f, const char* cmd_name,
                     const char* usage, unsigned int keyarg=1)
  {
    pkg->addCommand( makeVecCmd(pkg->interp(), f, cmd_name, usage, keyarg) );
  }

} // end namespace Tcl

static const char vcid_functor_h[] = "$Header$";
#endif // !FUNCTOR_H_DEFINED
