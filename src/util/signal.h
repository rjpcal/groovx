///////////////////////////////////////////////////////////////////////
//
// signal.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:29:04 1999
// written: Tue Aug 21 15:22:42 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIGNAL_H_DEFINED
#define SIGNAL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
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
 * Along with Signal, implements the Slot design pattern. An Slot can
 * be informed of changes in an Signal by calling connect() on that
 * Signal. Thereafter, the Slot will receive notifications of changes
 * in the Signal via receiveSignal().
 *
 **/
///////////////////////////////////////////////////////////////////////

class Util::Slot : public virtual Util::Object {
public:
  /// Virtual destructor.
  virtual ~Slot();

  template<class C, class MF>
  static Util::SoftRef<Util::Slot> make(C* obj, MF mf);

  /// Informs the Slot that one of its subjects has changed.
  virtual void receiveSignal() = 0;
};

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
  void connect(Util::SoftRef<Util::Slot> slot);

  /** Connect an object to this Signal, so that when the signal is
      triggered, \a mem_func will be called on \a obj. \c connect()
      returns the Util::UID of the connection object that is
      created. */
  template <class C, class MF>
  void connect(C* obj, MF mem_func);

  /// Remove a Slot from the list of those watching this Signal.
  void disconnect(Util::SoftRef<Util::Slot> slot);

  /** Informs all this object's Slots that this Signal's state
      has changed */
  void emitSignal() const;

private:

  Signal(const Signal&);
  Signal& operator=(const Signal&);

  class SigImpl;

  SigImpl* itsImpl;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * SlotAdapter class implements the slot interface from a target
 * object and a member function to apply to that object.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C, class MF>
class Util::SlotAdapter : public Util::Slot
{
  Util::SoftRef<C> itsObject;
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


///////////////////////////////////////////////////////////////////////
//
// Inline function definitions
//
///////////////////////////////////////////////////////////////////////

template <class C, class MF>
inline Util::SoftRef<Util::Slot> Util::Slot::make(C* obj, MF mf)
{
  return Util::SoftRef<Util::Slot>
    (Util::SlotAdapter<C, MF>::make(obj, mf));
};

template <class C, class MF>
inline void Util::Signal::connect(C* obj, MF mem_func)
{
  connect(Slot::make(obj, mem_func));
}

static const char vcid_signal_h[] = "$Header$";
#endif // !SIGNAL_H_DEFINED
