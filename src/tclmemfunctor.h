///////////////////////////////////////////////////////////////////////
//
// tclmemfunctor.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul 13 09:07:00 2001
// written: Thu Aug  9 18:43:12 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLMEMFUNCTOR_H_DEFINED
#define TCLMEMFUNCTOR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REF_H_DEFINED)
#include "util/ref.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCONVERT_H_DEFINED)
#include "tcl/tclconvert.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLFUNCTOR_H_DEFINED)
#include "tcl/tclfunctor.h"
#endif

namespace Tcl
{

///////////////////////////////////////////////////////////////////////
//
// Convert<> specialization allows us to get/return Ref<>'s
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
  struct Convert<WeakRef<T> >
  {
    static WeakRef<T> fromTcl(Tcl_Obj* obj)
    {
      Util::UID uid = Convert<Util::UID>::fromTcl(obj);
      return WeakRef<T>(uid);
    }

    static Tcl::ObjPtr toTcl(WeakRef<T> obj)
    {
      return Convert<Util::UID>::toTcl(obj.id());
    }
  };

///////////////////////////////////////////////////////////////////////
//
// MemFunctor wraps member functions and exposes them as ordinary
// operator() calls, using a Ref<> to pass the target object
//
///////////////////////////////////////////////////////////////////////

  template <class R, class C, class F>
  class MemFunctor {
  public:
    typedef F MemFunc;

    MemFunctor(MemFunc f) : itsMemFunc(f) {}

    R operator()(WeakRef<C> obj)
    {
      return (obj.get()->*itsMemFunc)();
    }

    template <class P1>
    R operator()(WeakRef<C> obj, P1 p1)
    {
      return (obj.get()->*itsMemFunc)(p1);
    }

    template <class P1, class P2>
    R operator()(WeakRef<C> obj, P1 p1, P2 p2)
    {
      return (obj.get()->*itsMemFunc)(p1, p2);
    }

    template <class P1, class P2, class P3>
    R operator()(WeakRef<C> obj, P1 p1, P2 p2, P3 p3)
    {
      return (obj.get()->*itsMemFunc)(p1, p2, p3);
    }

    template <class P1, class P2, class P3, class P4>
    R operator()(WeakRef<C> obj, P1 p1, P2 p2, P3 p3, P4 p4)
    {
      return (obj.get()->*itsMemFunc)(p1, p2, p3, p4);
    }

  private:
    MemFunc itsMemFunc;
  };


///////////////////////////////////////////////////////////////////////
//
// FuncTraits specializations for member functions
//
///////////////////////////////////////////////////////////////////////

  template <class R, class C>
  struct FuncTraits<R (C::*)()>
  {
    enum { numArgs = 1 };
    typedef WeakRef<C> Arg1_t;
  };

  template <class R, class C, class P1>
  struct FuncTraits<R (C::*)(P1)>
  {
    enum { numArgs = 2 };
    typedef WeakRef<C> Arg1_t;
    typedef P1 Arg2_t;
  };

  template <class R, class C, class P1, class P2>
  struct FuncTraits<R (C::*)(P1, P2)>
  {
    enum { numArgs = 3 };
    typedef WeakRef<C> Arg1_t;
    typedef P1 Arg2_t;
    typedef P2 Arg3_t;
  };

  template <class R, class C, class P1, class P2, class P3>
  struct FuncTraits<R (C::*)(P1, P2, P3)>
  {
    enum { numArgs = 4 };
    typedef WeakRef<C> Arg1_t;
    typedef P1 Arg2_t;
    typedef P2 Arg3_t;
    typedef P3 Arg4_t;
  };

  template <class R, class C, class P1, class P2, class P3, class P4>
  struct FuncTraits<R (C::*)(P1, P2, P3, P4)>
  {
    enum { numArgs = 4 };
    typedef WeakRef<C> Arg1_t;
    typedef P1 Arg2_t;
    typedef P2 Arg3_t;
    typedef P3 Arg4_t;
    typedef P4 Arg5_t;
  };

  template <class R, class C, class MF>
  struct FuncTraits<MemFunctor<R,C,MF> > : public FuncTraits<MF> {};


///////////////////////////////////////////////////////////////////////
//
// wrapFunc specializations for member functions
//
///////////////////////////////////////////////////////////////////////

  template <class R, class C>
  inline Functor1<R, MemFunctor<R,C, R (C::*)()> >
  wrapFunc(R (C::*mf)())
  {
    return MemFunctor<R,C, R (C::*)()>(mf);
  }

  template <class R, class C, class P1>
  inline Functor2<R, MemFunctor<R,C, R (C::*)(P1)> >
  wrapFunc(R (C::*mf)(P1))
  {
    return MemFunctor<R,C, R (C::*)(P1)>(mf);
  }

  template <class R, class C, class P1, class P2>
  inline Functor3<R, MemFunctor<R,C, R (C::*)(P1, P2)> >
  wrapFunc(R (C::*mf)(P1,P2))
  {
    return MemFunctor<R,C, R (C::*)(P1, P2)>(mf);
  }

  template <class R, class C, class P1, class P2, class P3>
  inline Functor4<R, MemFunctor<R,C, R (C::*)(P1, P2, P3)> >
  wrapFunc(R (C::*mf)(P1,P2,P3))
  {
    return MemFunctor<R,C, R (C::*)(P1, P2, P3)>(mf);
  }

  template <class R, class C, class P1, class P2, class P3, class P4>
  inline Functor5<R, MemFunctor<R,C, R (C::*)(P1, P2, P3, P4)> >
  wrapFunc(R (C::*mf)(P1,P2,P3,P4))
  {
    return MemFunctor<R,C, R (C::*)(P1, P2, P3, P4)>(mf);
  }

}

static const char vcid_tclmemfunctor_h[] = "$Header$";
#endif // !TCLMEMFUNCTOR_H_DEFINED
