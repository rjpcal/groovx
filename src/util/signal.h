///////////////////////////////////////////////////////////////////////
//
// observable.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 25 18:29:04 1999
// written: Mon Dec  6 14:00:55 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVABLE_H_DEFINED
#define OBSERVABLE_H_DEFINED

class Observer;
class ObservableImpl;

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

class Observable {
public:
  ///
  virtual ~Observable();

  /// Add an Observer to the list of those watching this Observable.
  void attach(Observer* obs);
  /// Remove an Observer from the list of those watching this Observable.
  void detach(Observer* obs);

protected:
  /** Constructor is protected so that Observable cannot be directly
		instantiated.  */
  Observable();

  /** Informs all this object's Observers that this Observable's state
		has changed */
  void sendStateChangeMsg() const;

private:
  /** Calls receiveDestroyMsg() on all of this object's
      observers. This is done only in Observable's destructor. */
  void sendDestroyMsg();

  ObservableImpl& itsImpl;
};

static const char vcid_observable_h[] = "$Header$";
#endif // !OBSERVABLE_H_DEFINED
