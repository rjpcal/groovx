///////////////////////////////////////////////////////////////////////
//
// signal.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:29:04 1999
// written: Tue Aug 21 13:21:52 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIGNAL_H_DEFINED
#define SIGNAL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBSERVER_H_DEFINED)
#include "util/observer.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REF_H_DEFINED)
#include "util/ref.h"
#endif

namespace Util
{
  class Observer;
  template <class C, class MF> class ObserverAdapter;
  class Signal;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Along with Observer, implements the Observer design
 * pattern. Subclasses of Signal do not have to override any virtual
 * functions to become an Signal: all of the machinery is provided in
 * this base class. However, to conform to the expected behavior,
 * subclasses of Signal must call emitSignal() when any of
 * their non-mutable properties changes. This in turn will call
 * receiveSignal() on all of the Observer's that are observing
 * this Signal.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Util::Signal {
public:
  /// Default constructor.
  Signal();

  /// Virtual destructor.
  virtual ~Signal();

  /// Add an Observer to the list of those watching this Signal.
  Util::UID connect(Observer* obs);

  /** Connect an object to this Signal, so that when the signal is
      triggered, \a mem_func will be called on \a obj. \c connect()
      returns the Util::UID of the connection object that is
      created. */
  template <class C, class MF>
  Util::UID connect(C* obj, MF mem_func);

  /// Remove an Observer from the list of those watching this Signal.
  void disconnect(Util::UID obs);

  /** Informs all this object's Observers that this Signal's state
      has changed */
  void emitSignal() const;

private:
  // Returns the id of the Observer object.
  Util::UID doConnect(Util::WeakRef<Util::Observer> obs);

  Signal(const Signal&);
  Signal& operator=(const Signal&);

  class SigImpl;

  SigImpl* itsImpl;
};

template <class C, class MF>
class Util::ObserverAdapter : public Util::Observer
{
  Util::WeakRef<Util::Object> itsObject;
  MF itsMemFunc;

  ObserverAdapter(C* obj, MF mf) : itsObject(obj, Util::WEAK), itsMemFunc(mf) {}

public:
  static Util::ObserverAdapter<C, MF>* make(C* obj, MF mf)
  { return new Util::ObserverAdapter<C, MF>(obj, mf); }

  virtual void receiveSignal()
  {
    if (itsObject.isValid())
      (itsObject.get()->*itsMemFunc)();
  }
};

template <class C, class MF>
Util::WeakRef<Util::Observer> makeObserver(C* obj, MF mf)
{
  return Util::WeakRef<Util::Observer>
    (Util::ObserverAdapter<C, MF>::make(obj, mf));
};

template <class C, class MF>
inline Util::UID Util::Signal::connect(C* obj, MF mem_func)
{
  return doConnect(makeObserver(obj, mem_func));
}

static const char vcid_signal_h[] = "$Header$";
#endif // !SIGNAL_H_DEFINED
