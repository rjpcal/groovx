///////////////////////////////////////////////////////////////////////
//
// signal.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:39:27 1999
// written: Tue Aug 21 14:55:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIGNAL_CC_DEFINED
#define SIGNAL_CC_DEFINED

#include "util/signal.h"

#include "util/dlink_list.h"
#include "util/ref.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Slot members
//
///////////////////////////////////////////////////////////////////////

Util::Slot::~Slot() {
DOTRACE("Util::Slot::~Slot");
}

///////////////////////////////////////////////////////////////////////
//
// Signal::SigImpl class
//
///////////////////////////////////////////////////////////////////////

namespace
{
  typedef Util::WeakRef<Util::Slot> ObsRef;

  typedef dlink_list<ObsRef> ListType;
}

struct Util::Signal::SigImpl
{
  SigImpl() : itsSlots() {}

  ListType itsSlots;
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

void Util::Signal::disconnect(Util::WeakRef<Util::Slot> slot)
{
DOTRACE("Util::Signal::disconnect");
  if (!slot.isValid()) return;

  itsImpl->itsSlots.remove(slot);

  DebugEvalNL(itsImpl->itsSlots.size());
}

void Util::Signal::connect(Util::WeakRef<Util::Slot> slot)
{
DOTRACE("Util::Signal::connect");
  if (!slot.isValid()) return;

  itsImpl->itsSlots.push_back(ObsRef(slot.get(), Util::WEAK));

  DebugEvalNL(itsImpl->itsSlots.size());
}

void Util::Signal::emitSignal() const
{
DOTRACE("Util::Signal::emitSignal");

  for (ListType::iterator
         ii = itsImpl->itsSlots.begin(),
         end = itsImpl->itsSlots.end();
       ii != end;
       /* incr in loop */)
    {
      DebugPrintNL("sending state change...");
      if ((*ii).isValid())
        {
          (*ii)->receiveSignal();
          ++ii;
        }
      else
        {
          ListType::iterator erase_me = ii;
          ++ii;
          itsImpl->itsSlots.erase(erase_me);
        }
    }
}

static const char vcid_signal_cc[] = "$Header$";
#endif // !SIGNAL_CC_DEFINED
