///////////////////////////////////////////////////////////////////////
//
// observer.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 25 18:37:02 1999
// written: Mon Dec  6 14:01:08 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVER_H_DEFINED
#define OBSERVER_H_DEFINED

class Observable;

///////////////////////////////////////////////////////////////////////
/**
 *
 * Along with Observable, implements the Observer design pattern. An
 * Observer can be informed of changes in an Observable by calling
 * attach() on that Observable. Thereafter, the Observer will receive
 * notifications of changes in the Observable via receiveStateChange()
 * and receiveDestroyMsg().
 *
 * @short Along with Observable, implements the Observer design pattern.
 **/
///////////////////////////////////////////////////////////////////////

class Observer {
public:
  ///
  virtual ~Observer();

  /// Informs the Observer that one of its subjects has changed.
  virtual void receiveStateChangeMsg(const Observable* obj) = 0;

  /// Informs the Observer that one of its subjects is being destroyed.
  virtual void receiveDestroyMsg(const Observable* obj) = 0;
};

static const char vcid_observer_h[] = "$Header$";
#endif // !OBSERVER_H_DEFINED
