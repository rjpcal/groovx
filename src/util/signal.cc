///////////////////////////////////////////////////////////////////////
//
// signal.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:39:27 1999
// written: Tue Aug 21 13:32:47 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIGNAL_CC_DEFINED
#define SIGNAL_CC_DEFINED

#include "util/signal.h"

#include "util/dlink_list.h"
#include "util/observer.h"
#include "util/ref.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Signal::SigImpl class
//
///////////////////////////////////////////////////////////////////////

namespace
{
  typedef Util::WeakRef<Util::Observer> ObsRef;

  typedef dlink_list<ObsRef> ListType;
}

struct Util::Signal::SigImpl
{
  SigImpl() : itsObservers() {}

  ListType itsObservers;
};

///////////////////////////////////////////////////////////////////////
//
// Signal method definitions
//
///////////////////////////////////////////////////////////////////////

Util::Signal::Signal() :
  itsImpl(new SigImpl)
{
DOTRACE("Util::Signal::Signal");
}

Util::Signal::~Signal()
{
DOTRACE("Util::Signal::~Signal");
  delete itsImpl;
}

Util::UID Util::Signal::connect(Util::Observer* obs)
{
DOTRACE("Util::Signal::connect");
  return doConnect(ObsRef(obs, Util::WEAK));
}

void Util::Signal::disconnect(Util::UID obs)
{
DOTRACE("Util::Signal::disconnect");
  if (obs == 0) return;

  ObsRef ref(obs, Util::WEAK);

  itsImpl->itsObservers.remove(ref);

  DebugEvalNL(itsImpl->itsObservers.size());
}

Util::UID Util::Signal::doConnect(Util::WeakRef<Util::Observer> obs)
{
DOTRACE("Util::Signal::doConnect");
  if (!obs.isValid()) return 0;

  itsImpl->itsObservers.push_back(obs);

  DebugEvalNL(itsImpl->itsObservers.size());

  return obs.id();
}

void Util::Signal::emitSignal() const
{
DOTRACE("Util::Signal::emitSignal");

  for (ListType::iterator
         ii = itsImpl->itsObservers.begin(),
         end = itsImpl->itsObservers.end();
       ii != end;
       ++ii)
    {
      DebugPrintNL("sending state change...");
      if ((*ii).isValid())
        (*ii)->receiveSignal();
    }
}

static const char vcid_signal_cc[] = "$Header$";
#endif // !SIGNAL_CC_DEFINED
