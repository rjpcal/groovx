///////////////////////////////////////////////////////////////////////
//
// signal.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:29:04 1999
// written: Mon Nov 25 12:31:52 2002
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

/// Slot implements the Slot design pattern along with Signal.

/** An Slot can be informed of changes in an Signal by calling connect() on
    that Signal. Thereafter, the Slot will receive notifications of any
    emit()'ed Signal's via call(). */

class Slot0 : public SlotBase
{
public:
  /// Default constructor.
  Slot0();

  /// Virtual destructor.
  virtual ~Slot0();

  template<class C, class MF>
  static Util::SoftRef<Slot0> make(C* obj, MF mf);

  /** Answers whether the components of this Slot still exist. This
      allows SlotAdapter, for example, to indicate if its target
      object has disappeared. */
  virtual bool exists() const = 0;

  virtual void doCall(void* /*params*/) { call(); }

  /// Informs the Slot that one of its subjects has changed.
  virtual void call() = 0;
};


//  ###################################################################
//  ===================================================================

/// SignalBase provides basic implementation for Signal.

class SignalBase : public Util::VolatileObject
{
protected:
  SignalBase();
  virtual ~SignalBase();

  void doEmit(void* params) const;

  /// Add a Slot to the list of those watching this Signal.
  void connect(Util::SoftRef<SlotBase> slot);

  /// Remove a Slot from the list of those watching this Signal.
  void disconnect(Util::SoftRef<SlotBase> slot);

private:
  SignalBase(const SignalBase&);
  SignalBase& operator=(const SignalBase&);

  class Impl;
  Impl* rep;
};


//  ###################################################################
//  ===================================================================

/// Signal implements the Slot design pattern along with Slot.

/** Classes that need to notify others of changes should hold a
    Util::Signal object by value, and call Signal::emit() when it is
    necessary to notify observers of the change. In turn, emit() will \c
    call() all of the Slot's that are observing this Signal. */

class Signal0 : public SignalBase
{
public:
  /// Default constructor.
  Signal0();

  /// Virtual destructor.
  virtual ~Signal0();

  /// Add a Slot to the list of those watching this Signal.
  void connect(Util::SoftRef<Slot0> slot);

  /** Connect an object to this Signal, so that when the signal is
      triggered, \a mem_func will be called on \a obj. \c connect()
      returns the Util::UID of the connection object that is
      created. */
  template <class C, class MF>
  void connect(C* obj, MF mem_func);

  /// Remove a Slot from the list of those watching this Signal.
  void disconnect(Util::SoftRef<Slot0> slot);

  /** Informs all this object's Slots that this Signal's state
      has changed */
  void emit() const { SignalBase::doEmit((void*)0); }

  /// Returns a slot which when call()'ed will cause this Signal to emit().
  Util::SoftRef<Slot0> slot() const { return slotEmitSelf; }

private:
  Signal0(const Signal0&);
  Signal0& operator=(const Signal0&);

  Util::Ref<Util::Slot0> slotEmitSelf;
};


//  ###################################################################
//  ===================================================================

/// SlotAdapter helps build a Slot from a target object and a member function.

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


//  ###################################################################
//  ===================================================================

// Inline function definitions

template <class C, class MF>
inline Util::SoftRef<Slot0> Slot0::make(C* obj, MF mf)
{
  return Util::SoftRef<Slot0>(SlotAdapter0<C, MF>::make(obj, mf),
                             Util::STRONG,
                             Util::PRIVATE);
};

template <class C, class MF>
inline void Signal0::connect(C* obj, MF mem_func)
{
  connect(Slot0::make(obj, mem_func));
}

} // end namespace Util

static const char vcid_signal_h[] = "$Header$";
#endif // !SIGNAL_H_DEFINED
