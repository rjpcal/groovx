///////////////////////////////////////////////////////////////////////
//
// observable.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:39:27 1999
// written: Tue Aug 21 11:33:12 2001
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

struct Util::Observable::ObsImpl
{
  ListType itsObservers;
};

///////////////////////////////////////////////////////////////////////
//
// Observable method definitions
//
///////////////////////////////////////////////////////////////////////

Util::Observable::Observable() :
  itsImpl(new ObsImpl)
{
DOTRACE("Util::Observable::Observable");
}

Util::Observable::~Observable()
{
DOTRACE("Util::Observable::~Observable");
  delete itsImpl;
}

void Util::Observable::attach(Util::Observer* obs)
{
DOTRACE("Util::Observable::attach");
  if (!obs) return;
  itsImpl->itsObservers.push_back(ObsRef(obs, Util::WEAK));
  DebugEvalNL(itsImpl->itsObservers.size());
}

void Util::Observable::detach(Util::Observer* obs)
{
DOTRACE("Util::Observable::detach");
  if (!obs) return;
  itsImpl->itsObservers.remove(ObsRef(obs, Util::WEAK));
  DebugEvalNL(itsImpl->itsObservers.size());
}

void Util::Observable::sendStateChangeMsg() const
{
DOTRACE("Util::Observable::sendStateChangeMsg");

  for (ListType::iterator
         ii = itsImpl->itsObservers.begin(),
         end = itsImpl->itsObservers.end();
       ii != end;
       ++ii)
    {
      DebugPrintNL("sending state change...");
      if ((*ii).isValid())
        (*ii)->receiveStateChangeMsg();
    }
}

static const char vcid_observable_cc[] = "$Header$";
#endif // !OBSERVABLE_CC_DEFINED
