///////////////////////////////////////////////////////////////////////
//
// functors.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Sep  7 15:07:16 2001
// written: Fri Sep 13 11:57:23 2002
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

//  ###################################################################
//  ===================================================================
//
//  FuncTraits
//
//  ===================================================================

  struct Null_t;

  /// Holds typedefs for the types of a function's arguments and return value.
  template <class R = void,
            class A1 = Null_t, class A2 = Null_t, class A3 = Null_t,
            class A4 = Null_t, class A5 = Null_t, class A6 = Null_t,
            class A7 = Null_t, class A8 = Null_t>
  struct FuncArgs
  {
    typedef R  Retn_t;
    typedef A1 Arg1_t;
    typedef A2 Arg2_t;
    typedef A3 Arg3_t;
    typedef A4 Arg4_t;
    typedef A5 Arg5_t;
    typedef A6 Arg6_t;
    typedef A7 Arg7_t;
    typedef A8 Arg8_t;
  };

  /// A traits class for holding information about functions/functors.
  template <class Func>
  struct FuncTraits
  {
    typedef typename Func::Retn_t Retn_t;
  };

//  ===================================================================
//
//  FuncTraits specializations for free functions
//
//  ===================================================================

  /// Specialization for free functions with no arguments.
  template <class R>
  struct FuncTraits<R (*)()>
    :
    public FuncArgs<R>
  {
    enum { numArgs = 0 };
  };

  /// Specialization for free functions with 1 argument.
  template <class R, class P1>
  struct FuncTraits<R (*)(P1)>
    :
    public FuncArgs<R, P1>
  {
    enum { numArgs = 1 };
  };

  /// Specialization for free functions with 2 arguments.
  template <class R, class P1, class P2>
  struct FuncTraits<R (*)(P1, P2)>
    :
    public FuncArgs<R, P1, P2>
  {
    enum { numArgs = 2 };
  };

  /// Specialization for free functions with 3 arguments.
  template <class R, class P1, class P2, class P3>
  struct FuncTraits<R (*)(P1, P2, P3)>
    :
    public FuncArgs<R, P1, P2, P3>
  {
    enum { numArgs = 3 };
  };

  /// Specialization for free functions with 4 arguments.
  template <class R, class P1, class P2, class P3, class P4>
  struct FuncTraits<R (*)(P1, P2, P3, P4)>
    :
    public FuncArgs<R, P1, P2, P3, P4>
  {
    enum { numArgs = 4 };
  };

  /// Specialization for free functions with 5 arguments.
  template <class R, class P1, class P2, class P3, class P4, class P5>
  struct FuncTraits<R (*)(P1, P2, P3, P4, P5)>
    :
    public FuncArgs<R, P1, P2, P3, P4, P5>
  {
    enum { numArgs = 5 };
  };

  /// Specialization for free functions with 6 arguments.
  template <class R, class P1, class P2, class P3, class P4, class P5, class P6>
  struct FuncTraits<R (*)(P1, P2, P3, P4, P5, P6)>
    :
    public FuncArgs<R, P1, P2, P3, P3, P4, P5, P6>
  {
    enum { numArgs = 6 };
  };

//  ===================================================================
//
//  FuncTraits specializations for member functions
//
//  We treat the "this" pointer as an implicit first-argument to the
//  function.
//
//  ===================================================================

  /// Specialization for member functions with "this" plus 0 arguments.
  template <class R, class C>
  struct FuncTraits<R (C::*)()>
    :
    public FuncArgs<R, Null_t>
  {
    enum { numArgs = 1 };
    typedef C Class_t;
  };

  /// Specialization for member functions with "this" plus 0 arguments.
  template <class R, class C>
  struct FuncTraits<R (C::*)() const>
    :
    public FuncArgs<R, Null_t>
  {
    enum { numArgs = 1 };
    typedef C Class_t;
  };

  /// Specialization for member functions with "this" plus 1 argument.
  template <class R, class C, class P1>
  struct FuncTraits<R (C::*)(P1)>
    :
    public FuncArgs<R, Null_t, P1>
  {
    enum { numArgs = 2 };
    typedef C Class_t;
  };

  /// Specialization for member functions with "this" plus 1 argument.
  template <class R, class C, class P1>
  struct FuncTraits<R (C::*)(P1) const>
    :
    public FuncArgs<R, Null_t, P1>
  {
    enum { numArgs = 2 };
    typedef C Class_t;
  };

  /// Specialization for member functions with "this" plus 2 arguments.
  template <class R, class C, class P1, class P2>
  struct FuncTraits<R (C::*)(P1, P2)>
    :
    public FuncArgs<R, Null_t, P1, P2>
  {
    enum { numArgs = 3 };
    typedef C Class_t;
  };

  /// Specialization for member functions with "this" plus 2 arguments.
  template <class R, class C, class P1, class P2>
  struct FuncTraits<R (C::*)(P1, P2) const>
    :
    public FuncArgs<R, Null_t, P1, P2>
  {
    enum { numArgs = 3 };
    typedef C Class_t;
  };

  /// Specialization for member functions with "this" plus 3 arguments.
  template <class R, class C, class P1, class P2, class P3>
  struct FuncTraits<R (C::*)(P1, P2, P3)>
    :
    public FuncArgs<R, Null_t, P1, P2, P3>
  {
    enum { numArgs = 4 };
    typedef C Class_t;
  };

  /// Specialization for member functions with "this" plus 3 arguments.
  template <class R, class C, class P1, class P2, class P3>
  struct FuncTraits<R (C::*)(P1, P2, P3) const>
    :
    public FuncArgs<R, Null_t, P1, P2, P3>
  {
    enum { numArgs = 4 };
    typedef C Class_t;
  };

  /// Specialization for member functions with "this" plus 4 arguments.
  template <class R, class C, class P1, class P2, class P3, class P4>
  struct FuncTraits<R (C::*)(P1, P2, P3, P4)>
    :
    public FuncArgs<R, Null_t, P1, P2, P3, P4>
  {
    enum { numArgs = 5 };
    typedef C Class_t;
  };

  /// Specialization for member functions with "this" plus 4 arguments.
  template <class R, class C, class P1, class P2, class P3, class P4>
  struct FuncTraits<R (C::*)(P1, P2, P3, P4) const>
    :
    public FuncArgs<R, Null_t, P1, P2, P3, P4>
  {
    enum { numArgs = 5 };
    typedef C Class_t;
  };

//  ###################################################################
//  ===================================================================

/// FuncHolder is a helper class to make pointer semantics explicit.
/** FuncHolder helps to avoid overeager compiler warnings about holding a
    pointer without defining dtor, copy-ctor, and operator=(). If a class
    instead inherits privately from FuncHolder, such warnings will be
    avoided. */

//  ===================================================================

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

//  ###################################################################
//  ===================================================================

/// MemFunctorBase adapts a member function to an ordinary operator().
/** The "this" pointer is passed through the first argument of the
    operator() call, via a raw pointer or a SoftRef<>. */

//  ===================================================================

  template <class F>
  class MemFunctorBase;

  /// FuncTraits specialization for MemFunctorBase.
  template <class MF>
  struct FuncTraits<MemFunctorBase<MF> > : public FuncTraits<MF>
  {};

  template <class C>
  C* extractPtr(C* c) { return c; }

  template <class C>
  C* extractPtr(const SoftRef<C>& c) { return c.get(); }

  template <class MemFunc>
  class MemFunctorBase : private FuncHolder<MemFunc>,
                         public FuncTraits<MemFunctorBase<MemFunc> >
  {
  public:
    typedef typename FuncTraits<MemFunc>::Retn_t R;
    typedef typename FuncTraits<MemFunc>::Class_t C;

    MemFunctorBase(MemFunc f) : FuncHolder<MemFunc>(f) {}

    template <class Ptr>
    R operator()(Ptr obj)
    {
      return (extractPtr(obj)->*itsHeldFunc)();
    }

    template <class Ptr, class P1>
    R operator()(Ptr obj, P1 p1)
    {
      return (extractPtr(obj)->*itsHeldFunc)(p1);
    }

    template <class Ptr, class P1, class P2>
    R operator()(Ptr obj, P1 p1, P2 p2)
    {
      return (extractPtr(obj)->*itsHeldFunc)(p1, p2);
    }

    template <class Ptr, class P1, class P2, class P3>
    R operator()(Ptr obj, P1 p1, P2 p2, P3 p3)
    {
      return (extractPtr(obj)->*itsHeldFunc)(p1, p2, p3);
    }

    template <class Ptr, class P1, class P2, class P3, class P4>
    R operator()(Ptr obj, P1 p1, P2 p2, P3 p3, P4 p4)
    {
      return (extractPtr(obj)->*itsHeldFunc)(p1, p2, p3, p4);
    }
  };

//  ###################################################################
//  ===================================================================

/// MemFunctor extends MemFunctorBase using SoftRef<> for "this" pointers.

//  ===================================================================

  template <class MemFunc>
  struct MemFunctor : public MemFunctorBase<MemFunc>
  {
    MemFunctor(MemFunc f) : MemFunctorBase<MemFunc>(f) {}

    // operator()'s inherited from MemFunctorBase
  };

  /// Specialization of FuncTraits for MemFunctor.
  template <class MF>
  struct FuncTraits<MemFunctor<MF> > : public FuncTraits<MF>
  {
    typedef SoftRef<typename MemFunctor<MF>::C> Arg1_t;
  };

  /// Factory function to make a MemFunctor from any member function.
  template <class MF>
  inline MemFunctor<MF> memFunc(MF mf)
  {
    return mf;
  }


//  ###################################################################
//  ===================================================================

  template <class BaseFunctor, class Bound>
  class BoundFirst;

  /// FuncTraits specialization for BoundFirst.
  template <class BaseFunctor, class Bound>
  struct FuncTraits<BoundFirst<BaseFunctor, Bound> >
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

/// BoundFirst wraps another functor type with a fixed first argument.
/** BoundFirst's can be constructed with the factory function
    bindFirst(). */

  template <class BaseFunctor, class Bound_t>
  class BoundFirst : private FuncHolder<BaseFunctor>,
                     public FuncTraits<BoundFirst<BaseFunctor, Bound_t> >
  {
  public:
    BoundFirst(BaseFunctor base, Bound_t bound) :
      FuncHolder<BaseFunctor>(base),
      itsBound(bound)
    {}

    BoundFirst(const BoundFirst& other) :
      FuncHolder<BaseFunctor>(other),
      itsBound(other.itsBound)
    {}

    // Workaround for g++-3.1's overeager "implicit typename" warnings
    typedef FuncTraits<BoundFirst<BaseFunctor, Bound_t> > Traits;
    typedef typename Traits::Retn_t   Retn_t;
    typedef typename Traits::Arg1_t   Arg1_t;
    typedef typename Traits::Arg2_t   Arg2_t;
    typedef typename Traits::Arg3_t   Arg3_t;
    typedef typename Traits::Arg4_t   Arg4_t;
    typedef typename Traits::Arg5_t   Arg5_t;
    typedef typename Traits::Arg6_t   Arg6_t;
    typedef typename Traits::Arg7_t   Arg7_t;
    typedef typename Traits::Arg8_t   Arg8_t;

    //
    // All versions of operator() are provided, but only the one that
    // involves the correct call to itsHeldFunc() will compile
    // successfully
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
    BoundFirst& operator=(const BoundFirst&);

    Bound_t itsBound;
  };

  /// Factory function for creating BoundFirst functors.
  template <class BaseFunctor, class Bound_t>
  BoundFirst<BaseFunctor, Bound_t> bindFirst(BaseFunctor base, Bound_t bound)
  {
    return BoundFirst<BaseFunctor, Bound_t>(base, bound);
  }


//  ###################################################################
//  ===================================================================

  template <class BaseFunctor, class Bound>
  class BoundLast;

  /// FuncTraits specialization for BoundLast.
  template <class BaseFunctor, class Bound>
  struct FuncTraits<BoundLast<BaseFunctor, Bound> >
  {
    enum { numArgs = (FuncTraits<BaseFunctor>::numArgs-1) };

    typedef typename FuncTraits<BaseFunctor>::Retn_t   Retn_t;
    typedef typename FuncTraits<BaseFunctor>::Arg1_t   Arg1_t;
    typedef typename FuncTraits<BaseFunctor>::Arg2_t   Arg2_t;
    typedef typename FuncTraits<BaseFunctor>::Arg3_t   Arg3_t;
    typedef typename FuncTraits<BaseFunctor>::Arg4_t   Arg4_t;
    typedef typename FuncTraits<BaseFunctor>::Arg5_t   Arg5_t;
    typedef typename FuncTraits<BaseFunctor>::Arg6_t   Arg6_t;
    typedef typename FuncTraits<BaseFunctor>::Arg7_t   Arg7_t;
    typedef typename FuncTraits<BaseFunctor>::Arg8_t   Arg8_t;
  };

  /// BoundLast wraps another functor type with a fixed last argument.
  /** BoundLast objects can be constructed with the factory function
      bindLast(). */

  template <class BaseFunctor, class Bound_t>
  class BoundLast : private FuncHolder<BaseFunctor>,
                    public FuncTraits<BoundLast<BaseFunctor, Bound_t> >
  {
  public:
    BoundLast(BaseFunctor base, Bound_t bound) :
      FuncHolder<BaseFunctor>(base),
      itsBound(bound)
    {}

    BoundLast(const BoundLast& other) :
      FuncHolder<BaseFunctor>(other),
      itsBound(other.itsBound)
    {}

    // Workaround for g++-3.1's overeager "implicit typename" warnings
    typedef FuncTraits<BoundLast<BaseFunctor, Bound_t> > Traits;
    typedef typename Traits::Retn_t   Retn_t;
    typedef typename Traits::Arg1_t   Arg1_t;
    typedef typename Traits::Arg2_t   Arg2_t;
    typedef typename Traits::Arg3_t   Arg3_t;
    typedef typename Traits::Arg4_t   Arg4_t;
    typedef typename Traits::Arg5_t   Arg5_t;
    typedef typename Traits::Arg6_t   Arg6_t;
    typedef typename Traits::Arg7_t   Arg7_t;
    typedef typename Traits::Arg8_t   Arg8_t;

    //
    // All versions of operator() are provided, but only the one that
    // involves the correct call to itsHeldFunc() will compile
    // successfully
    //

    Retn_t operator()()
    {
      return itsHeldFunc(itsBound);
    }
    Retn_t operator()(Arg1_t p1)
    {
      return itsHeldFunc(p1, itsBound);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2)
    {
      return itsHeldFunc(p1, p2, itsBound);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2, Arg3_t p3)
    {
      return itsHeldFunc(p1, p2, p3, itsBound);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2, Arg3_t p3, Arg4_t p4)
    {
      return itsHeldFunc(p1, p2, p3, p4, itsBound);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2, Arg3_t p3, Arg4_t p4,
                      Arg5_t p5)
    {
      return itsHeldFunc(p1, p2, p3, p4, p5, itsBound);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2, Arg3_t p3, Arg4_t p4, Arg5_t p5,
                      Arg6_t p6)
    {
      return itsHeldFunc(p1, p2, p3, p4, p5, p6, itsBound);
    }
    Retn_t operator()(Arg1_t p1, Arg2_t p2, Arg3_t p3, Arg4_t p4, Arg5_t p5,
                      Arg6_t p6, Arg7_t p7)
    {
      return itsHeldFunc(p1, p2, p3, p4, p5, p6, p7, itsBound);
    }

  private:
    BoundLast& operator=(const BoundLast&);

    Bound_t itsBound;
  };

  /// Factory function for creating BoundLast functors.
  template <class BaseFunctor, class Bound_t>
  BoundLast<BaseFunctor, Bound_t> bindLast(BaseFunctor base, Bound_t bound)
  {
    return BoundLast<BaseFunctor, Bound_t>(base, bound);
  }

} // end namespace Util

static const char vcid_functors_h[] = "$Header$";
#endif // !FUNCTORS_H_DEFINED
