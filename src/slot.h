///////////////////////////////////////////////////////////////////////
//
// observer.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 25 18:37:02 1999
// written: Wed May 26 09:49:56 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVER_H_DEFINED
#define OBSERVER_H_DEFINED

class Observable;

///////////////////////////////////////////////////////////////////////
//
// Observer interface
//
///////////////////////////////////////////////////////////////////////

class Observer {
public:
  virtual ~Observer();

  virtual void receiveStateChangeMsg(const Observable* obj) = 0;
  virtual void receiveDestroyMsg(const Observable* obj) = 0;
};

static const char vcid_observer_h[] = "$Header$";
#endif // !OBSERVER_H_DEFINED
