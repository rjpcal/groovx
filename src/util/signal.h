///////////////////////////////////////////////////////////////////////
//
// signal.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue May 25 18:29:04 1999
// written: Wed Mar 19 13:10:48 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIGNAL_H_DEFINED
#define SIGNAL_H_DEFINED

#include "util/object.h"
#include "util/ref.h"
#include "util/volatileobject.h"

namespace Util
{
  class Slot0;
  template <class C, class MF> class SlotAdapter0;
  class Signal0;

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

//  ###################################################################
//  ===================================================================

/// The base class for all slot classes.

/** Slots can be triggered from a Signal by calling connect() on that
    Signal. Thereafter, the slot will be called whenever that signal emits
    a message. */

class SlotBase : public virtual Util::Object
{
public:
  /// Default constructor.
  SlotBase();

  /// Virtual destructor.
  virtual ~SlotBase();

  /** Answers whether the components of this Slot still exist. This
      allows SlotAdapter, for example, to indicate if its target
      object has disappeared. */
  virtual bool exists() const = 0;

  /// Call the slot with the given arguments
  virtual void doCall(void* params) = 0;
};

//  ###################################################################
//  ===================================================================

/// A zero-argument slot class.

class Slot0 : public SlotBase
{
public:
  /// Default constructor.
  Slot0();

  /// Virtual destructor.
  virtual ~Slot0();

  template<class C, class MF>
  static Util::SoftRef<Slot0> make(C* obj, MF mf);

  virtual void call() = 0;

  /// Unpacks any parameters and then calls the implementation.
  virtual void doCall(void* /*params*/) { call(); }
};


//  ###################################################################
//  ===================================================================

/// A mem-func adapter for zer-argument slots.

template <class C, class MF>
class SlotAdapter0 : public Slot0
{
  Util::SoftRef<C> itsObject;
  MF itsMemFunc;

  SlotAdapter0(C* obj, MF mf) :
    itsObject(obj, Util::WEAK, Util::PRIVATE), itsMemFunc(mf) {}

public:
  static SlotAdapter0<C, MF>* make(C* obj, MF mf)
  { return new SlotAdapter0<C, MF>(obj, mf); }

  virtual bool exists() const { return itsObject.isValid(); }

  virtual void call()
  {
    if (itsObject.isValid())
      (itsObject.get()->*itsMemFunc)();
  }
};

template <class C, class MF>
inline Util::SoftRef<Slot0> Slot0::make(C* obj, MF mf)
{
  return Util::SoftRef<Slot0>(SlotAdapter0<C, MF>::make(obj, mf),
                             Util::STRONG,
                             Util::PRIVATE);
};

//  ###################################################################
//  ===================================================================

/// A one-argument call data wrapper.

template <class P1>
struct CallData1
{
  CallData1(P1 i1) : p1(i1) {}
  P1 p1;
};

//  ###################################################################
//  ===================================================================

/// A slot with one argument.

template <class P1>
class Slot1 : public SlotBase
{
public:
  /// Default constructor.
  Slot1() {}

  /// Virtual destructor.
  virtual ~Slot1() {}

  template<class C, class MF>
  static Util::SoftRef<Slot1<P1> > make(C* obj, MF mf);

  virtual void call(P1 p1) = 0;

  /// Unpacks any parameters and then calls the implementation.
  virtual void doCall(void* params)
  {
    CallData1<P1>* data = static_cast<CallData1<P1>*>(params);
    call(data->p1);
  }
};

//  ###################################################################
//  ===================================================================

/// A mem-func adapter for slots with one argument.

template <class P1, class C, class MF>
class SlotAdapter1 : public Slot1<P1>
{
  Util::SoftRef<C> itsObject;
  MF itsMemFunc;

  SlotAdapter1(C* obj, MF mf) :
    itsObject(obj, Util::WEAK, Util::PRIVATE), itsMemFunc(mf) {}

public:
  static SlotAdapter1<P1, C, MF>* make(C* obj, MF mf)
  { return new SlotAdapter1<P1, C, MF>(obj, mf); }

  virtual bool exists() const { return itsObject.isValid(); }

  virtual void call(P1 p1)
  {
    if (itsObject.isValid())
      (itsObject.get()->*itsMemFunc)(p1);
  }
};

template <class P1>
template <class C, class MF>
inline Util::SoftRef<Slot1<P1> > Slot1<P1>::make(C* obj, MF mf)
{
  return Util::SoftRef<Slot1<P1> >(SlotAdapter1<P1, C, MF>::make(obj, mf),
                                   Util::STRONG,
                                   Util::PRIVATE);
};


//  ###################################################################
//  ===================================================================

/// SignalBase provides basic implementation for Signal.

/** Classes that need to notify others of changes should hold an
    appropriate signal object by value, and call emit() when it is
    necessary to notify observers of the change. In turn, emit() will \c
    call all of the Slot's that are observing this Signal. */

class SignalBase : public Util::VolatileObject
{
protected:
  SignalBase();
  virtual ~SignalBase();

  void doEmit(void* params) const;

  /// Add a Slot to the list of those watching this Signal.
  void doConnect(Util::SoftRef<SlotBase> slot);

  /// Remove a Slot from the list of those watching this Signal.
  void doDisconnect(Util::SoftRef<SlotBase> slot);

private:
  SignalBase(const SignalBase&);
  SignalBase& operator=(const SignalBase&);

  class Impl;
  Impl* rep;
};


//  ###################################################################
//  ===================================================================

/// A zero-argument signal.

class Signal0 : public SignalBase
{
public:
  /// Default constructor.
  Signal0();

  /// Virtual destructor.
  virtual ~Signal0();

  /// Add a Slot to the list of those watching this Signal.
  void connect(Util::SoftRef<Slot0> slot)
  { SignalBase::doConnect(slot); }

  /** Connect an object to this Signal, so that when the signal is
      triggered, \a mem_func will be called on \a obj. \c connect()
      returns the Util::UID of the connection object that is
      created. */
  template <class C, class MF>
  void connect(C* obj, MF mem_func)
  { connect(Slot0::make(obj, mem_func)); }

  /// Remove a Slot from the list of those watching this Signal.
  void disconnect(Util::SoftRef<Slot0> slot)
  { SignalBase::doDisconnect(slot); }

  /** Informs all this object's Slots that this Signal's state
      has changed */
  void emit() const { SignalBase::doEmit((void*)0); }

  /// Returns a slot which when called will cause this Signal to emit().
  Util::SoftRef<Slot0> slot() const { return slotEmitSelf; }

private:
  Signal0(const Signal0&);
  Signal0& operator=(const Signal0&);

  Util::Ref<Util::Slot0> slotEmitSelf;
};


//  ###################################################################
//  ===================================================================

/// A one-argument signal.

template <class P1>
class Signal1 : public SignalBase
{
public:
  Signal1() {}

  virtual ~Signal1() {}

  void connect(Util::SoftRef<Slot1<P1> > slot)
  { SignalBase::doConnect(slot); }

  template <class C, class MF>
  void connect(C* obj, MF mem_func)
  { connect(Slot1<P1>::make(obj, mem_func)); }

  void disconnect(Util::SoftRef<Slot1<P1> > slot)
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

} // end namespace Util

static const char vcid_signal_h[] = "$Header$";
#endif // !SIGNAL_H_DEFINED
