///////////////////////////////////////////////////////////////////////
//
// signal.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:29:04 1999
// written: Tue Aug 21 14:24:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIGNAL_H_DEFINED
#define SIGNAL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(SLOT_H_DEFINED)
#include "util/slot.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REF_H_DEFINED)
#include "util/ref.h"
#endif

namespace Util
{
  class Slot;
  template <class C, class MF> class SlotAdapter;
  class Signal;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Along with Slot, implements the Observer design pattern. Subclasses
 * of Signal do not have to override any virtual functions to become
 * an Signal: all of the machinery is provided in this base
 * class. However, to conform to the expected behavior, subclasses of
 * Signal must call emitSignal() when any of their non-mutable
 * properties changes. This in turn will call receiveSignal() on all
 * of the Slot's that are observing this Signal.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Util::Signal {
public:
  /// Default constructor.
  Signal();

  /// Virtual destructor.
  virtual ~Signal();

  /// Add a Slot to the list of those watching this Signal.
  Util::UID connect(Slot* obs);

  /** Connect an object to this Signal, so that when the signal is
      triggered, \a mem_func will be called on \a obj. \c connect()
      returns the Util::UID of the connection object that is
      created. */
  template <class C, class MF>
  Util::UID connect(C* obj, MF mem_func);

  /// Remove a Slot from the list of those watching this Signal.
  void disconnect(Util::UID obs);

  /** Informs all this object's Slots that this Signal's state
      has changed */
  void emitSignal() const;

private:
  // Returns the id of the Slot object.
  Util::UID doConnect(Util::WeakRef<Util::Slot> obs);

  Signal(const Signal&);
  Signal& operator=(const Signal&);

  class SigImpl;

  SigImpl* itsImpl;
};

template <class C, class MF>
class Util::SlotAdapter : public Util::Slot
{
  Util::WeakRef<C> itsObject;
  MF itsMemFunc;

  SlotAdapter(C* obj, MF mf) : itsObject(obj, Util::WEAK), itsMemFunc(mf) {}

public:
  static Util::SlotAdapter<C, MF>* make(C* obj, MF mf)
  { return new Util::SlotAdapter<C, MF>(obj, mf); }

  virtual void receiveSignal()
  {
    if (itsObject.isValid())
      (itsObject.get()->*itsMemFunc)();
  }
};

template <class C, class MF>
Util::WeakRef<Util::Slot> makeSlot(C* obj, MF mf)
{
  return Util::WeakRef<Util::Slot>
    (Util::SlotAdapter<C, MF>::make(obj, mf));
};

template <class C, class MF>
inline Util::UID Util::Signal::connect(C* obj, MF mem_func)
{
  return doConnect(makeSlot(obj, mem_func));
}

static const char vcid_signal_h[] = "$Header$";
#endif // !SIGNAL_H_DEFINED
