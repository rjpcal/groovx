///////////////////////////////////////////////////////////////////////
//
// observable.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:39:27 1999
// written: Wed Jun 13 16:32:54 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVABLE_CC_DEFINED
#define OBSERVABLE_CC_DEFINED

#include "util/observable.h"

#include "util/dlink_list.h"
#include "util/observer.h"
#include "util/ref.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Observable::ObsImpl class
//
///////////////////////////////////////////////////////////////////////

namespace
{
  typedef Util::WeakRef<Util::Observer> ObsRef;

  typedef dlink_list<ObsRef> ListType;
}

struct Util::Observable::ObsImpl {
public:
  ObsImpl() : itsObservers()
    { }
  ~ObsImpl()
    { }
  ListType itsObservers;
};

///////////////////////////////////////////////////////////////////////
//
// Observable method definitions
//
///////////////////////////////////////////////////////////////////////

Util::Observable::Observable() :
  itsImpl(*(new ObsImpl))
{
DOTRACE("Util::Observable::Observable");
}

Util::Observable::~Observable() {
DOTRACE("Util::Observable::~Observable");
  sendDestroyMsg();
  delete &itsImpl;
}

void Util::Observable::attach(Util::Observer* obs) {
DOTRACE("Util::Observable::attach");
  if (!obs) return;
  itsImpl.itsObservers.push_back(ObsRef(obs, Util::WEAK));
}

void Util::Observable::detach(Util::Observer* obs) {
DOTRACE("Util::Observable::detach");
  if (!obs) return;
  itsImpl.itsObservers.remove(ObsRef(obs, Util::WEAK));
}

void Util::Observable::sendStateChangeMsg() const {
DOTRACE("Util::Observable::sendStateChangeMsg");

  for (ListType::iterator
         ii = itsImpl.itsObservers.begin(),
         end = itsImpl.itsObservers.end();
       ii != end;
       ++ii)
    {
      if ((*ii).isValid())
        (*ii)->receiveStateChangeMsg(this);
    }
}

void Util::Observable::sendDestroyMsg() {
DOTRACE("Util::Observable::sendDestroyMsg");
  ListType& theList = itsImpl.itsObservers;

  DebugEval(itsImpl.itsObservers.size());

  // WARNING! This loop _cannot_ be done with a simple for loop that
  // iterates over the elements of theList, since theList is changed
  // within the loop by detach() (which removes an element from
  // theList), and therefore the loop iterator would be invalidated.
  while (!theList.empty()) {
    DebugEval(itsImpl.itsObservers.size());

    WeakRef<Util::Observer> obs = theList.front();

    DebugEval((void *) this);

    // Let the observer know that 'this' is being destroyed ...
    if (obs.isValid())
      {
        obs->receiveDestroyMsg(this);
      }

    // ... and remove it from the list of observers
    theList.pop_front();
  }
}

static const char vcid_observable_cc[] = "$Header$";
#endif // !OBSERVABLE_CC_DEFINED
