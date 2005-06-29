///////////////////////////////////////////////////////////////////////
//
// signal.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue May 25 18:29:04 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_NUB_SIGNAL_H_UTC20050626084019_DEFINED
#define GROOVX_NUB_SIGNAL_H_UTC20050626084019_DEFINED

#include "nub/object.h"
#include "nub/ref.h"
#include "nub/volatileobject.h"

namespace Nub
{
#if 0
  template <class R>
  class Marshal
  {
    R val;

  public:
    Marshal() : val() {}

    typedef R InType;
    typedef R OutType;

    bool marshal(const InType& in)
    {
      val = in;
    }

    OutType value() const { return val; }
  };
#endif

  //  #######################################################
  //  =======================================================

  /// The base class for all slot classes.

  /** Slots can be triggered from a Signal by calling connect() on
      that Signal. Thereafter, the slot will be called whenever that
      signal emits a message. */

  class SlotBase : public virtual Nub::Object
  {
  public:
    /// Default constructor.
    SlotBase();

    /// Virtual destructor.
    virtual ~SlotBase() throw();

    /// Answers whether the components of this Slot still exist.
    /** This allows a slot adapter, for example, to indicate if its
        target object has disappeared. Default implementation returns
        true always. */
    virtual bool exists() const;

    /// Call the slot with the given arguments
    virtual void doCall(void* params) = 0;
  };

  //  #######################################################
  //  =======================================================

  /// A zero-argument slot class.

  class Slot0 : public SlotBase
  {
  public:
    /// Default constructor.
    Slot0();

    /// Virtual destructor.
    virtual ~Slot0() throw();

    template <class C, class MF>
    static Nub::SoftRef<Slot0> make(C* obj, MF mf);

    static Nub::SoftRef<Slot0> make(void (*freeFunc)());

    virtual void call() = 0;

    /// Unpacks any parameters and then calls the implementation.
    virtual void doCall(void* /*params*/) { call(); }
  };


  //  #######################################################
  //  =======================================================

  /// A mem-func adapter for zero-argument slots.

  template <class C, class MF>
  class SlotAdapterMemFunc0 : public Slot0
  {
    Nub::SoftRef<C> itsObject;
    MF itsMemFunc;

    SlotAdapterMemFunc0(C* obj, MF mf) :
      itsObject(obj, Nub::WEAK, Nub::PRIVATE), itsMemFunc(mf) {}

    virtual ~SlotAdapterMemFunc0() throw() {}

  public:
    static SlotAdapterMemFunc0<C, MF>* make(C* obj, MF mf)
    { return new SlotAdapterMemFunc0<C, MF>(obj, mf); }

    virtual bool exists() const { return itsObject.isValid(); }

    virtual void call()
    {
      if (itsObject.isValid())
        (itsObject.get()->*itsMemFunc)();
    }
  };

  template <class C, class MF>
  inline Nub::SoftRef<Slot0> Slot0::make(C* obj, MF mf)
  {
    return Nub::SoftRef<Slot0>
      (SlotAdapterMemFunc0<C, MF>::make(obj, mf),
       Nub::STRONG,
       Nub::PRIVATE);
  }

  //  #######################################################
  //  =======================================================

  /// A mem-func adapter for zero-argument slots.

  class SlotAdapterFreeFunc0 : public Slot0
  {
  public:
    typedef void (FreeFunc)();

  private:
    FreeFunc* itsFreeFunc;

    SlotAdapterFreeFunc0(FreeFunc* f);

    virtual ~SlotAdapterFreeFunc0() throw();

  public:
    static SlotAdapterFreeFunc0* make(FreeFunc* f);

    virtual void call();
  };

  //  #######################################################
  //  =======================================================

  /// A one-argument call data wrapper.

  template <class P1>
  struct CallData1
  {
    CallData1(P1 i1) : p1(i1) {}
    P1 p1;
  };

  //  #######################################################
  //  =======================================================

  /// A slot with one argument.

  template <class P1>
  class Slot1 : public SlotBase
  {
  public:
    /// Default constructor.
    Slot1() {}

    /// Virtual destructor.
    virtual ~Slot1() throw() {}

    template <class C, class MF>
    static Nub::SoftRef<Slot1<P1> > make(C* obj, MF mf);

    template <class FF>
    static Nub::SoftRef<Slot1<P1> > make(FF f);

    virtual void call(P1 p1) = 0;

    /// Unpacks any parameters and then calls the implementation.
    virtual void doCall(void* params)
    {
      CallData1<P1>* data = static_cast<CallData1<P1>*>(params);
      call(data->p1);
    }
  };

  //  #######################################################
  //  =======================================================

  /// A mem-func adapter for slots with one argument.

  template <class P1, class C, class MF>
  class SlotAdapterMemFunc1 : public Slot1<P1>
  {
    Nub::SoftRef<C> itsObject;
    MF itsMemFunc;

    SlotAdapterMemFunc1(C* obj, MF mf) :
      itsObject(obj, Nub::WEAK, Nub::PRIVATE), itsMemFunc(mf) {}

    virtual ~SlotAdapterMemFunc1() throw() {}

  public:
    static SlotAdapterMemFunc1<P1, C, MF>* make(C* obj, MF mf)
    { return new SlotAdapterMemFunc1<P1, C, MF>(obj, mf); }

    virtual bool exists() const { return itsObject.isValid(); }

    virtual void call(P1 p1)
    {
      if (itsObject.isValid())
        (itsObject.get()->*itsMemFunc)(p1);
    }
  };

  template <class P1>
  template <class C, class MF>
  inline Nub::SoftRef<Slot1<P1> > Slot1<P1>::make(C* obj, MF mf)
  {
    return Nub::SoftRef<Slot1<P1> >
      (SlotAdapterMemFunc1<P1, C, MF>::make(obj, mf),
       Nub::STRONG,
       Nub::PRIVATE);
  }


  //  #######################################################
  //  =======================================================

  /// A free-func adapter for slots with one argument.

  template <class P1, class FF>
  class SlotAdapterFreeFunc1 : public Slot1<P1>
  {
    FF itsFreeFunc;

    SlotAdapterFreeFunc1(FF f) : itsFreeFunc(f) {}

    virtual ~SlotAdapterFreeFunc1() throw() {}

  public:
    static SlotAdapterFreeFunc1<P1, FF>* make(FF f)
    { return new SlotAdapterFreeFunc1<P1, FF>(f); }

    virtual void call(P1 p1)
    {
      (*itsFreeFunc)(p1);
    }
  };

  template <class P1>
  template <class FF>
  inline Nub::SoftRef<Slot1<P1> > Slot1<P1>::make(FF f)
  {
    return Nub::SoftRef<Slot1<P1> >
      (SlotAdapterFreeFunc1<P1, FF>::make(f),
       Nub::STRONG,
       Nub::PRIVATE);
  }


  //  #######################################################
  //  =======================================================

  /// SignalBase provides basic implementation for Signal.

  /** Classes that need to notify others of changes should hold an
      appropriate signal object by value, and call emit() when it is
      necessary to notify observers of the change. In turn, emit()
      will \c call all of the Slot's that are observing this
      Signal. */

  class SignalBase : public Nub::VolatileObject
  {
  protected:
    SignalBase();
    virtual ~SignalBase() throw();

    void doEmit(void* params) const;

    /// Add a Slot to the list of those watching this Signal.
    void doConnect(Nub::SoftRef<SlotBase> slot);

    /// Remove a Slot from the list of those watching this Signal.
    void doDisconnect(Nub::SoftRef<SlotBase> slot);

  private:
    SignalBase(const SignalBase&);
    SignalBase& operator=(const SignalBase&);

    class Impl;
    Impl* rep;
  };


  //  #######################################################
  //  =======================================================

  /// A zero-argument signal.

  class Signal0 : public SignalBase
  {
  public:
    /// Default constructor.
    Signal0();

    /// Virtual destructor.
    virtual ~Signal0() throw();

    /// Add a Slot to the list of those watching this Signal.
    Nub::SoftRef<Slot0> connect(Nub::SoftRef<Slot0> slot)
    { SignalBase::doConnect(slot); return slot; }

    /// Connect a free function to this Signal0.
    Nub::SoftRef<Slot0> connect(void (*freeFunc)())
    { return connect(Slot0::make(freeFunc)); }

    /// Connect an object to this Signal0.
    /** After connection, when the signal is triggered, \a mem_func
        will be called on \a obj. \c connect() returns the Nub::UID of
        the connection object that is created. */
    template <class C, class MF>
    Nub::SoftRef<Slot0> connect(C* obj, MF mem_func)
    { return connect(Slot0::make(obj, mem_func)); }

    /// Remove a Slot from the list of those watching this Signal0.
    void disconnect(Nub::SoftRef<Slot0> slot)
    { SignalBase::doDisconnect(slot); }

    /// Trigger all of this object's Slots.
    void emit() const { SignalBase::doEmit(static_cast<void*>(0)); }

    /// Returns a slot which when called will cause this Signal to emit().
    Nub::SoftRef<Slot0> slot() const { return slotEmitSelf; }

  private:
    Signal0(const Signal0&);
    Signal0& operator=(const Signal0&);

    Nub::Ref<Nub::Slot0> slotEmitSelf;
  };


  //  #######################################################
  //  =======================================================

  /// A one-argument signal.

  template <class P1>
  class Signal1 : public SignalBase
  {
  public:
    Signal1() {}

    virtual ~Signal1() throw() {}

    Nub::SoftRef<Slot1<P1> > connect(Nub::SoftRef<Slot1<P1> > slot)
    { SignalBase::doConnect(slot); return slot; }

    Nub::SoftRef<Slot1<P1> > connect(void (*free_func)(P1))
    { return connect(Slot1<P1>::make(free_func)); }

    template <class C, class MF>
    Nub::SoftRef<Slot1<P1> > connect(C* obj, MF mem_func)
    { return connect(Slot1<P1>::make(obj, mem_func)); }

    void disconnect(Nub::SoftRef<Slot1<P1> > slot)
    { SignalBase::doDisconnect(slot); }

    void emit(P1 p1) const
    {
      CallData1<P1> dat( p1 );
      SignalBase::doEmit(static_cast<void*>(&dat));
    }

  private:
    Signal1(const Signal1&);
    Signal1& operator=(const Signal1&);
  };

} // end namespace Nub

static const char vcid_groovx_nub_signal_h_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_SIGNAL_H_UTC20050626084019_DEFINED
