///////////////////////////////////////////////////////////////////////
//
// observable.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 25 18:39:27 1999
// written: Wed Mar 15 10:17:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVABLE_CC_DEFINED
#define OBSERVABLE_CC_DEFINED

#include "observable.h"

#include <list>

#include "observer.h"

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// ObservableImpl class
//
///////////////////////////////////////////////////////////////////////

struct ObservableImpl {
public:
  ObservableImpl() : itsObservers()
	 { DOTRACE("ObservableImpl::ObservableImpl"); }
  ~ObservableImpl()
	 { DOTRACE("ObservableImpl::~ObservableImpl"); }
  list<Observer *> itsObservers;
};

///////////////////////////////////////////////////////////////////////
//
// Observable method definitions
//
///////////////////////////////////////////////////////////////////////

Observable::Observable() :
  itsImpl(*(new ObservableImpl))
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
  list<Observer *>& theList = itsImpl.itsObservers;

  DebugEval(itsImpl.itsObservers.size());

  // WARNING! This loop _cannot_ be done with a simple for loop that
  // iterates over the elements of theList, since theList is changed
  // within the loop by detach() (which removes an element from
  // theList), and therefore the loop iterator would be invalidated.
  while (!theList.empty()) {
	 DebugEval(itsImpl.itsObservers.size());

	 Observer* obs = theList.front();

	 DebugEval((void *) this);
	 DebugEvalNL((void *) obs);

	 // Let the observer know that 'this' is being destroyed ...
	 obs->receiveDestroyMsg(this);
	 // ... and remove it from the list of observers
	 detach(obs);
  }
}

static const char vcid_observable_cc[] = "$Header$";
#endif // !OBSERVABLE_CC_DEFINED
