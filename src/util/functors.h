///////////////////////////////////////////////////////////////////////
//
// functors.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Sep  7 15:07:16 2001
// written: Fri Sep  7 16:15:55 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FUNCTORS_H_DEFINED
#define FUNCTORS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REF_H_DEFINED)
#include "util/ref.h"
#endif

namespace Util
{

///////////////////////////////////////////////////////////////////////
//
// FuncTraits
//
///////////////////////////////////////////////////////////////////////

  struct Null_t;

  struct FuncTraitsBase
  {
    typedef Null_t Arg1_t;
    typedef Null_t Arg2_t;
    typedef Null_t Arg3_t;
    typedef Null_t Arg4_t;
    typedef Null_t Arg5_t;
    typedef Null_t Arg6_t;
    typedef Null_t Arg7_t;
    typedef Null_t Arg8_t;
  };

  template <class Func>
  struct FuncTraits
  {
    typedef typename Func::Retn_t Retn_t;
  };

///////////////////////////////////////////////////////////////////////
//
// FuncTraits specializations for free functions
//
///////////////////////////////////////////////////////////////////////

  template <class R>
  struct FuncTraits<R (*)()> : public FuncTraitsBase
  {
    enum { numArgs = 0 };
    typedef R Retn_t;
  };

  template <class R, class P1>
  struct FuncTraits<R (*)(P1)> : public FuncTraitsBase
  {
    enum { numArgs = 1 };
    typedef R Retn_t;
    typedef P1 Arg1_t;
  };

  template <class R, class P1, class P2>
  struct FuncTraits<R (*)(P1, P2)> : public FuncTraitsBase
  {
    enum { numArgs = 2 };
    typedef R Retn_t;
    typedef P1 Arg1_t;
    typedef P2 Arg2_t;
  };

  template <class R, class P1, class P2, class P3>
  struct FuncTraits<R (*)(P1, P2, P3)> : public FuncTraitsBase
  {
    enum { numArgs = 3 };
    typedef R Retn_t;
    typedef P1 Arg1_t;
    typedef P2 Arg2_t;
    typedef P3 Arg3_t;
  };

  template <class R, class P1, class P2, class P3, class P4>
  struct FuncTraits<R (*)(P1, P2, P3, P4)> : public FuncTraitsBase
  {
    enum { numArgs = 4 };
    typedef R Retn_t;
    typedef P1 Arg1_t;
    typedef P2 Arg2_t;
    typedef P3 Arg3_t;
    typedef P4 Arg4_t;
  };

  template <class R, class P1, class P2, class P3, class P4, class P5>
  struct FuncTraits<R (*)(P1, P2, P3, P4, P5)> : public FuncTraitsBase
  {
    enum { numArgs = 5 };
    typedef R Retn_t;
    typedef P1 Arg1_t;
    typedef P2 Arg2_t;
    typedef P3 Arg3_t;
    typedef P4 Arg4_t;
    typedef P5 Arg5_t;
  };

  template <class R, class P1, class P2, class P3, class P4, class P5, class P6>
  struct FuncTraits<R (*)(P1, P2, P3, P4, P5, P6)> : public FuncTraitsBase
  {
    enum { numArgs = 6 };
    typedef R Retn_t;
    typedef P1 Arg1_t;
    typedef P2 Arg2_t;
    typedef P3 Arg3_t;
    typedef P4 Arg4_t;
    typedef P5 Arg5_t;
    typedef P6 Arg6_t;
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
    typedef R Retn_t;
    typedef C* Arg1_t;
  };

  template <class R, class C, class P1>
  struct FuncTraits<R (C::*)(P1)>
  {
    enum { numArgs = 2 };
    typedef R Retn_t;
    typedef C* Arg1_t;
    typedef P1 Arg2_t;
  };

  template <class R, class C, class P1, class P2>
  struct FuncTraits<R (C::*)(P1, P2)>
  {
    enum { numArgs = 3 };
    typedef R Retn_t;
    typedef C* Arg1_t;
    typedef P1 Arg2_t;
    typedef P2 Arg3_t;
  };

  template <class R, class C, class P1, class P2, class P3>
  struct FuncTraits<R (C::*)(P1, P2, P3)>
  {
    enum { numArgs = 4 };
    typedef R Retn_t;
    typedef C* Arg1_t;
    typedef P1 Arg2_t;
    typedef P2 Arg3_t;
    typedef P3 Arg4_t;
  };

  template <class R, class C, class P1, class P2, class P3, class P4>
  struct FuncTraits<R (C::*)(P1, P2, P3, P4)>
  {
    enum { numArgs = 5 };
    typedef R Retn_t;
    typedef C* Arg1_t;
    typedef P1 Arg2_t;
    typedef P2 Arg3_t;
    typedef P3 Arg4_t;
    typedef P4 Arg5_t;
  };

///////////////////////////////////////////////////////////////////////
//
// FuncHolder is basically a little helper class to avoid overeager
// compiler warnings about holding a pointer without defining dtor,
// copy-ctor, and operator=(). If a class instead inherits privately
// from FuncHolder, such warnings will be avoided.
//
///////////////////////////////////////////////////////////////////////

  template <class Func>
  struct FuncHolder
  {
    FuncHolder<Func>(Func f) : itsHeldFunc(f) {}

    FuncHolder<Func>(const FuncHolder<Func>& other) :
      itsHeldFunc(other.itsHeldFunc) {}

    FuncHolder<Func>& operator=(const FuncHolder<Func>& other)
    {
      itsHeldFunc = other.itsHeldFunc; return *this;
    }

    Func itsHeldFunc;
  };

///////////////////////////////////////////////////////////////////////
//
// MemFunctor wraps a member function and exposes it through ordinary
// operator() calls, using a SoftRef<> to pass the target object
//
///////////////////////////////////////////////////////////////////////

  template <class R, class C, class F>
  class MemFunctor;

  template <class R, class C, class MF>
  struct FuncTraits<MemFunctor<R,C,MF> > : public FuncTraits<MF>
  {
    typedef SoftRef<C> Arg1_t;
  };

  template <class R, class C, class MemFunc>
  class MemFunctor : private FuncHolder<MemFunc>,
                     public FuncTraits<MemFunctor<R, C, MemFunc> >
  {
  public:
    MemFunctor(MemFunc f) : FuncHolder<MemFunc>(f) {}

    R operator()(SoftRef<C> obj)
    {
      return (obj.get()->*itsHeldFunc)();
    }

    template <class P1>
    R operator()(SoftRef<C> obj, P1 p1)
    {
      return (obj.get()->*itsHeldFunc)(p1);
    }

    template <class P1, class P2>
    R operator()(SoftRef<C> obj, P1 p1, P2 p2)
    {
      return (obj.get()->*itsHeldFunc)(p1, p2);
    }

    template <class P1, class P2, class P3>
    R operator()(SoftRef<C> obj, P1 p1, P2 p2, P3 p3)
    {
      return (obj.get()->*itsHeldFunc)(p1, p2, p3);
    }

    template <class P1, class P2, class P3, class P4>
    R operator()(SoftRef<C> obj, P1 p1, P2 p2, P3 p3, P4 p4)
    {
      return (obj.get()->*itsHeldFunc)(p1, p2, p3, p4);
    }
  };


///////////////////////////////////////////////////////////////////////
//
// A BoundFirst functor wraps another functor type with a fixed value
// for its first argument. BoundFirst's can be constructed with the
// helper template function bindFirst().
//
///////////////////////////////////////////////////////////////////////

  template <class BaseFunctor>
  class BoundFirst;

  template <class BaseFunctor>
  struct FuncTraits<BoundFirst<BaseFunctor> >
  {
    enum { numArgs = (FuncTraits<BaseFunctor>::numArgs-1) };

    typedef typename FuncTraits<BaseFunctor>::Retn_t   Retn_t;
    typedef typename FuncTraits<BaseFunctor>::Arg2_t   Arg1_t;
    typedef typename FuncTraits<BaseFunctor>::Arg3_t   Arg2_t;
    typedef typename FuncTraits<BaseFunctor>::Arg4_t   Arg3_t;
    typedef typename FuncTraits<BaseFunctor>::Arg5_t   Arg4_t;
    typedef typename FuncTraits<BaseFunctor>::Arg6_t   Arg5_t;
    typedef typename FuncTraits<BaseFunctor>::Arg7_t   Arg6_t;
    typedef typename FuncTraits<BaseFunctor>::Arg8_t   Arg7_t;
    typedef                                   Null_t   Arg8_t;
  };

  template <class BaseFunctor>
  class BoundFirst : private FuncHolder<BaseFunctor>,
                     public FuncTraits<BoundFirst<BaseFunctor> >
  {
  public:
    typedef typename FuncTraits<BaseFunctor>::Arg1_t Bound_t;

    BoundFirst(BaseFunctor base, Bound_t bound) :
      FuncHolder<BaseFunctor>(base),
      itsBound(bound)
    {}

    //
    // All versions of operator() are provided, but only the one that
    // involves the correct call to itsFunc() will compile successfully
    //

    Retn_t operator()()
    {
      return itsHeldFunc(itsBound);
    }
    Retn_t operator()(Arg1_t p1)
    {
      return itsHeldFunc(itsBound, p1);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2)
    {
      return itsHeldFunc(itsBound, p1, p2);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2, Arg3_t p3)
    {
      return itsHeldFunc(itsBound, p1, p2, p3);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2, Arg3_t p3, Arg4_t p4)
    {
      return itsHeldFunc(itsBound, p1, p2, p3, p4);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2, Arg3_t p3, Arg4_t p4,
                      Arg5_t p5)
    {
      return itsHeldFunc(itsBound, p1, p2, p3, p4, p5);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2, Arg3_t p3, Arg4_t p4, Arg5_t p5,
                      Arg6_t p6)
    {
      return itsHeldFunc(itsBound, p1, p2, p3, p4, p5, p6);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2, Arg3_t p3, Arg4_t p4, Arg5_t p5,
                      Arg6_t p6, Arg7_t p7)
    {
      return itsHeldFunc(itsBound, p1, p2, p3, p4, p5, p6, p7);
    }

  private:
    Bound_t itsBound;
  };

  template <class BaseFunctor>
  BoundFirst<BaseFunctor>
  bindFirst(BaseFunctor base, typename BoundFirst<BaseFunctor>::Bound_t bound)
  {
    return BoundFirst<BaseFunctor>(base, bound);
  }

} // end namespace Util

static const char vcid_functors_h[] = "$Header$";
#endif // !FUNCTORS_H_DEFINED
