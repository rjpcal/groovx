///////////////////////////////////////////////////////////////////////
//
// signal.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:29:04 1999
// written: Mon Nov 25 10:57:33 2002
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
  class Slot;
  template <class C, class MF> class SlotAdapter;
  class Signal;

//  ###################################################################
//  ===================================================================

/// Slot implements the Slot design pattern along with Signal.

/** An Slot can be informed of changes in an Signal by calling connect() on
    that Signal. Thereafter, the Slot will receive notifications of any
    emit()'ed Signal's via call(). */

class Slot : public virtual Util::Object
{
public:
  /// Default constructor.
  Slot();

  /// Virtual destructor.
  virtual ~Slot();

  template<class C, class MF>
  static Util::SoftRef<Slot> make(C* obj, MF mf);

  /** Answers whether the components of this Slot still exist. This
      allows SlotAdapter, for example, to indicate if its target
      object has disappeared. */
  virtual bool exists() const = 0;

  /// Informs the Slot that one of its subjects has changed.
  virtual void call() = 0;
};


//  ###################################################################
//  ===================================================================

/// SignalBase provides basic implementation for Signal.

class SignalBase : public Util::VolatileObject
{
public:
  SignalBase();
  virtual ~SignalBase();

  void receive();
  void emit() const;

  /// Add a Slot to the list of those watching this Signal.
  void connect(Util::SoftRef<Slot> slot);

  /// Remove a Slot from the list of those watching this Signal.
  void disconnect(Util::SoftRef<Slot> slot);

  /// Returns a slot which when call()'ed will cause this Signal to emit().
  Util::SoftRef<Slot> slot() const;

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

class Signal : public SignalBase
{
public:
  /// Default constructor.
  Signal();

  /// Virtual destructor.
  virtual ~Signal();

  /// Add a Slot to the list of those watching this Signal.
  void connect(Util::SoftRef<Slot> slot);

  /** Connect an object to this Signal, so that when the signal is
      triggered, \a mem_func will be called on \a obj. \c connect()
      returns the Util::UID of the connection object that is
      created. */
  template <class C, class MF>
  void connect(C* obj, MF mem_func);

  /// Remove a Slot from the list of those watching this Signal.
  void disconnect(Util::SoftRef<Slot> slot);

  /** Informs all this object's Slots that this Signal's state
      has changed */
  void emit() const;

  /// Returns a slot which when call()'ed will cause this Signal to emit().
  Util::SoftRef<Slot> slot() const;

private:

  Signal(const Signal&);
  Signal& operator=(const Signal&);

  class SigImpl;

  SigImpl* itsImpl;
};


//  ###################################################################
//  ===================================================================

/// SlotAdapter helps build a Slot from a target object and a member function.

template <class C, class MF>
class SlotAdapter : public Slot
{
  Util::SoftRef<C> itsObject;
  MF itsMemFunc;

  SlotAdapter(C* obj, MF mf) :
    itsObject(obj, Util::WEAK, Util::PRIVATE), itsMemFunc(mf) {}

public:
  static SlotAdapter<C, MF>* make(C* obj, MF mf)
  { return new SlotAdapter<C, MF>(obj, mf); }

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
inline Util::SoftRef<Slot> Slot::make(C* obj, MF mf)
{
  return Util::SoftRef<Slot>(SlotAdapter<C, MF>::make(obj, mf),
                             Util::STRONG,
                             Util::PRIVATE);
};

template <class C, class MF>
inline void Signal::connect(C* obj, MF mem_func)
{
  connect(Slot::make(obj, mem_func));
}

} // end namespace Util

static const char vcid_signal_h[] = "$Header$";
#endif // !SIGNAL_H_DEFINED
