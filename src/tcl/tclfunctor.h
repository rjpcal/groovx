///////////////////////////////////////////////////////////////////////
//
// functor.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 22 09:07:27 2001
// written: Thu Jul 12 13:23:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FUNCTOR_H_DEFINED
#define FUNCTOR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCMD_H_DEFINED)
#include "tcl/tclcmd.h"
#endif

namespace Tcl {

  //
  // zero arguments
  //

  template <class R>
  class CmdP0 : public TclCmd {
  public:
    typedef R (*Func)();

    CmdP0<R>(Tcl_Interp* interp, Func f, const char* cmd_name,
             const char* usage) :
      TclCmd(interp, cmd_name, usage, 1),
      itsFunc(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      ctx.setResult(itsFunc());
    }

  private:
    CmdP0<R>(const CmdP0<R>&);
    CmdP0<R>& operator=(const CmdP0<R>&);

    Func itsFunc;
  };

  template <>
  void CmdP0<void>::invoke(Tcl::Context& ctx) { itsFunc(); }



  //
  // one argument
  //

  template <class R, class P1>
  class CmdP1 : public TclCmd {
  public:
    typedef R (*Func)(P1);

    CmdP1<R, P1>(Tcl_Interp* interp, Func f, const char* cmd_name,
                 const char* usage) :
      TclCmd(interp, cmd_name, usage, 2),
      itsFunc(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      P1 p1 = ctx.getValFromArg(1, (P1*)0);
      ctx.setResult(itsFunc(p1));
    }

  private:
    CmdP1<R, P1>(const CmdP1<R, P1>&);
    CmdP1<R, P1>& operator=(const CmdP1<R, P1>&);

    Func itsFunc;
  };

  template <class P1>
  class CmdP1<void, P1> : public TclCmd {
  public:
    typedef void (*Func)(P1);

    CmdP1<void, P1>(Tcl_Interp* interp, Func f, const char* cmd_name,
                    const char* usage) :
      TclCmd(interp, cmd_name, usage, 2),
      itsFunc(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      P1 p1 = ctx.getValFromArg(1, (P1*)0);
      itsFunc(p1);
    }

  private:
    CmdP1<void, P1>(const CmdP1<void, P1>&);
    CmdP1<void, P1>& operator=(const CmdP1<void, P1>&);

    Func itsFunc;
  };


  //
  // two arguments
  //

  template <class R, class P1, class P2>
  class CmdP2 : public TclCmd {
  public:
    typedef R (*Func)(P1, P2);

    CmdP2<R, P1, P2>(Tcl_Interp* interp, Func f, const char* cmd_name,
                     const char* usage) :
      TclCmd(interp, cmd_name, usage, 3),
      itsFunc(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      P1 p1 = ctx.getValFromArg(1, (P1*)0);
      P2 p2 = ctx.getValFromArg(2, (P2*)0);
      ctx.setResult(itsFunc(p1, p2));
    }

  private:
    CmdP2<R, P1, P2>(const CmdP2<R, P1, P2>&);
    CmdP2<R, P1, P2>& operator=(const CmdP2<R, P1, P2>&);

    Func itsFunc;
  };

  template <class P1, class P2>
  class CmdP2<void, P1, P2> : public TclCmd {
  public:
    typedef void (*Func)(P1, P2);

    CmdP2<void, P1, P2>(Tcl_Interp* interp, Func f, const char* cmd_name,
                        const char* usage) :
      TclCmd(interp, cmd_name, usage, 3),
      itsFunc(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      P1 p1 = ctx.getValFromArg(1, (P1*)0);
      P2 p2 = ctx.getValFromArg(2, (P2*)0);
      itsFunc(p1, p2);
    }

  private:
    CmdP2<void, P1, P2>(const CmdP2<void, P1, P2>&);
    CmdP2<void, P1, P2>& operator=(const CmdP2<void, P1, P2>&);

    Func itsFunc;
  };


  //
  // three arguments
  //

  template <class R, class P1, class P2, class P3>
  class CmdP3 : public TclCmd {
  public:
    typedef R (*Func)(P1, P2, P3);

    CmdP3<R, P1, P2, P3>(Tcl_Interp* interp, Func f, const char* cmd_name,
                         const char* usage) :
      TclCmd(interp, cmd_name, usage, 4),
      itsFunc(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      P1 p1 = ctx.getValFromArg(1, (P1*)0);
      P2 p2 = ctx.getValFromArg(2, (P2*)0);
      P3 p3 = ctx.getValFromArg(3, (P3*)0);
      ctx.setResult(itsFunc(p1, p2, p3));
    }

  private:
    CmdP3<R, P1, P2, P3>(const CmdP3<R, P1, P2, P3>&);
    CmdP3<R, P1, P2, P3>& operator=(const CmdP3<R, P1, P2, P3>&);

    Func itsFunc;
  };

  template <class P1, class P2, class P3>
  class CmdP3<void, P1, P2, P3> : public TclCmd {
  public:
    typedef void (*Func)(P1, P2, P3);

    CmdP3<void, P1, P2, P3>(Tcl_Interp* interp, Func f, const char* cmd_name,
                            const char* usage) :
      TclCmd(interp, cmd_name, usage, 4),
      itsFunc(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      P1 p1 = ctx.getValFromArg(1, (P1*)0);
      P2 p2 = ctx.getValFromArg(2, (P2*)0);
      P3 p3 = ctx.getValFromArg(3, (P3*)0);
      itsFunc(p1, p2, p3);
    }

  private:
    CmdP3<void, P1, P2, P3>(const CmdP3<void, P1, P2, P3>&);
    CmdP3<void, P1, P2, P3>& operator=(const CmdP3<void, P1, P2, P3>&);

    Func itsFunc;
  };


  //
  // four arguments
  //

  template <class R, class P1, class P2, class P3, class P4>
  class CmdP4 : public TclCmd {
  public:
    typedef R (*Func)(P1, P2, P3, P4);

    CmdP4<R, P1, P2, P3, P4>(Tcl_Interp* interp, Func f, const char* cmd_name,
                             const char* usage) :
      TclCmd(interp, cmd_name, usage, 5),
      itsFunc(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      P1 p1 = ctx.getValFromArg(1, (P1*)0);
      P2 p2 = ctx.getValFromArg(2, (P2*)0);
      P3 p3 = ctx.getValFromArg(3, (P3*)0);
      P4 p4 = ctx.getValFromArg(4, (P4*)0);
      ctx.setResult(itsFunc(p1, p2, p3, p4));
    }

  private:
    CmdP4<R, P1, P2, P3, P4>(const CmdP4<R, P1, P2, P3, P4>&);
    CmdP4<R, P1, P2, P3, P4>& operator=(const CmdP4<R, P1, P2, P3, P4>&);

    Func itsFunc;
  };

  template <class P1, class P2, class P3, class P4>
  class CmdP4<void, P1, P2, P3, P4> : public TclCmd {
  public:
    typedef void (*Func)(P1, P2, P3, P4);

    CmdP4<void, P1, P2, P3, P4>(Tcl_Interp* interp, Func f, const char* cmd_name,
                                const char* usage) :
      TclCmd(interp, cmd_name, usage, 5),
      itsFunc(f)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      P1 p1 = ctx.getValFromArg(1, (P1*)0);
      P2 p2 = ctx.getValFromArg(2, (P2*)0);
      P3 p3 = ctx.getValFromArg(3, (P3*)0);
      P4 p4 = ctx.getValFromArg(4, (P4*)0);
      itsFunc(p1, p2, p3, p4);
    }

  private:
    CmdP4<void, P1, P2, P3, P4>(const CmdP4<void, P1, P2, P3, P4>&);
    CmdP4<void, P1, P2, P3, P4>& operator=(const CmdP4<void, P1, P2, P3, P4>&);

    Func itsFunc;
  };


  template <class Func>
  inline TclCmd* makeCmd(Tcl_Interp* interp, Func f,
                         const char* cmd_name, const char* usage);

  template <class R>
  inline TclCmd* makeCmd(Tcl_Interp* interp,
                         R (*f)(),
                         const char* cmd_name, const char* usage)
  {
    return new CmdP0<R>(interp, f, cmd_name, usage);
  }

  template <class R, class P1>
  inline TclCmd* makeCmd(Tcl_Interp* interp,
                         R (*f)(P1), // typename CmdP1<P1>::Func
                         const char* cmd_name, const char* usage)
  {
    return new CmdP1<R, P1>(interp, f, cmd_name, usage);
  }

  template <class R, class P1, class P2>
  inline TclCmd* makeCmd(Tcl_Interp* interp,
                         R (*f)(P1, P2),
                         const char* cmd_name, const char* usage)
  {
    return new CmdP2<R, P1, P2>(interp, f, cmd_name, usage);
  }

  template <class R, class P1, class P2, class P3>
  inline TclCmd* makeCmd(Tcl_Interp* interp,
                         R (*f)(P1, P2, P3),
                         const char* cmd_name, const char* usage)
  {
    return new CmdP3<R, P1, P2, P3>(interp, f, cmd_name, usage);
  }

  template <class R, class P1, class P2, class P3, class P4>
  inline TclCmd* makeCmd(Tcl_Interp* interp,
                         R (*f)(P1, P2, P3, P4),
                         const char* cmd_name, const char* usage)
  {
    return new CmdP4<R, P1, P2, P3, P4>(interp, f, cmd_name, usage);
  }

} // end namespace Tcl

static const char vcid_functor_h[] = "$Header$";
#endif // !FUNCTOR_H_DEFINED
