///////////////////////////////////////////////////////////////////////
//
// observable.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:39:27 1999
// written: Tue May 22 12:19:17 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVABLE_CC_DEFINED
#define OBSERVABLE_CC_DEFINED

#include "util/observable.h"

#include "util/dlink_list.h"
#include "util/observer.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// ObservableImpl class
//
///////////////////////////////////////////////////////////////////////

namespace {
  typedef dlink_list<Observer*> ListType;
}

struct ObservableImpl {
public:
  ObservableImpl() : itsObservers()
	 { }
  ~ObservableImpl()
	 { }
  ListType itsObservers;
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
  DebugEvalNL((void*)obs);
  itsImpl.itsObservers.push_back(obs);
}

void Observable::detach(Observer* obs) {
DOTRACE("Observable::detach");
  if (!obs) return;
  itsImpl.itsObservers.remove(obs);
}

void Observable::sendStateChangeMsg() const {
DOTRACE("Observable::sendStateChangeMsg");
  for (ListType::iterator ii = itsImpl.itsObservers.begin();
		 ii != itsImpl.itsObservers.end();
		 ii++) {
	 DebugEvalNL((void*)*ii);
	 Assert(*ii != 0);
	 (*ii)->receiveStateChangeMsg(this);
  }
}

void Observable::sendDestroyMsg() {
DOTRACE("Observable::sendDestroyMsg");
  ListType& theList = itsImpl.itsObservers;

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

	 Assert(obs != 0);

	 // Let the observer know that 'this' is being destroyed ...
	 obs->receiveDestroyMsg(this);
	 // ... and remove it from the list of observers
	 detach(obs);
  }
}

static const char vcid_observable_cc[] = "$Header$";
#endif // !OBSERVABLE_CC_DEFINED
