///////////////////////////////////////////////////////////////////////
//
// observable.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:29:04 1999
// written: Mon Aug 13 16:39:06 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVABLE_H_DEFINED
#define OBSERVABLE_H_DEFINED

namespace Util
{
  class Observer;
  class Observable;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Along with Observer, implements the Observer design
 * pattern. Subclasses of Observable do not have to override any
 * virtual functions to become an Observable: all of the machinery is
 * provided in this base class. However, to conform to the expected
 * behavior, subclasses of Observable must call sendStateChangeMsg()
 * when any of their non-mutable properties changes. This in turn will
 * call receiveStateChangeMsg() on all of the Observer's that are
 * observing this Observable.
 *
 * @short Along with Observer, implements the Observer design pattern.
 **/
///////////////////////////////////////////////////////////////////////

class Util::Observable {
protected:
  /** Constructor is protected so that Observable cannot be directly
      instantiated.  */
  Observable();

public:
  /// Virtual destructor.
  virtual ~Observable();

  /// Add an Observer to the list of those watching this Observable.
  void attach(Observer* obs);
  /// Remove an Observer from the list of those watching this Observable.
  void detach(Observer* obs);

  /** Informs all this object's Observers that this Observable's state
      has changed */
  void sendStateChangeMsg() const;

private:
  class ObsImpl;

  ObsImpl* itsImpl;
};

static const char vcid_observable_h[] = "$Header$";
#endif // !OBSERVABLE_H_DEFINED
