///////////////////////////////////////////////////////////////////////
//
// signal.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:39:27 1999
// written: Tue Aug 21 11:45:44 2001
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

void Util::Signal::connect(Util::Observer* sig)
{
DOTRACE("Util::Signal::connect");
  if (!sig) return;
  itsImpl->itsObservers.push_back(ObsRef(sig, Util::WEAK));
  DebugEvalNL(itsImpl->itsObservers.size());
}

void Util::Signal::disconnect(Util::Observer* sig)
{
DOTRACE("Util::Signal::disconnect");
  if (!sig) return;
  itsImpl->itsObservers.remove(ObsRef(sig, Util::WEAK));
  DebugEvalNL(itsImpl->itsObservers.size());
}

void Util::Signal::sendStateChangeMsg() const
{
DOTRACE("Util::Signal::sendStateChangeMsg");

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

static const char vcid_signal_cc[] = "$Header$";
#endif // !SIGNAL_CC_DEFINED
