///////////////////////////////////////////////////////////////////////
//
// observable.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 25 18:39:27 1999
// written: Wed May 26 11:07:32 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVABLE_CC_DEFINED
#define OBSERVABLE_CC_DEFINED

#include "observable.h"

#include <list>

#include "observer.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// ObservableImpl class
//
///////////////////////////////////////////////////////////////////////

struct ObservableImpl {
public:
  ObservableImpl() : itsObservers() {}
  ~ObservableImpl() {}
  list<Observer *> itsObservers;
};

///////////////////////////////////////////////////////////////////////
//
// Observable method definitions
//
///////////////////////////////////////////////////////////////////////

Observable::Observable() :
  itsImpl(*(new ObservableImpl));
{
DOTRACE("Observable::Observable");
}

Observable::~Observable() {
DOTRACE("Observable::~Observable");
  sendDestroyMsg();
  delete &itsImpl;
}

void Observable::attach(Observer* obs) {
DOTRACE("Observable::attach");
  if (!obs) return;
  itsImpl.itsObservers.push_back(obs);
}

void Observable::detach(Observer* obs) {
DOTRACE("Observable::detach");
  if (!obs) return;
  itsImpl.itsObservers.remove(obs);
}

void Observable::sendStateChangeMsg() const {
DOTRACE("Observable::sendStateChangeMsg");
  for (list<Observer *>::iterator ii = itsImpl.itsObservers.begin();
		 ii != itsImpl.itsObservers.end();
		 ii++) {
	 (*ii)->receiveStateChangeMsg(this);
  }
}

void Observable::sendDestroyMsg() {
DOTRACE("Observable::sendDestroyMsg");
  for (list<Observer *>::iterator ii = itsImpl.itsObservers.begin();
		 ii != itsImpl.itsObservers.end();
		 ii++) {
	 // Let the observer know that 'this' is being destroyed ...
	 (*ii)->receiveDestroyMsg(this);
	 // ... and remove it from the list of observers
	 detach(*ii);
  }
}

static const char vcid_observable_cc[] = "$Header$";
#endif // !OBSERVABLE_CC_DEFINED
