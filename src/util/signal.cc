///////////////////////////////////////////////////////////////////////
//
// signal.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:39:27 1999
// written: Tue Nov 26 19:44:23 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIGNAL_CC_DEFINED
#define SIGNAL_CC_DEFINED

#include "util/signal.h"

#include "system/demangle.h"

#include "util/dlink_list.h"
#include "util/ref.h"

#include <typeinfo>

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// SlotBase members
//
///////////////////////////////////////////////////////////////////////

Util::SlotBase::SlotBase()
{
DOTRACE("Util::SlotBase::SlotBase");
}

Util::SlotBase::~SlotBase()
{
DOTRACE("Util::SlotBase::~SlotBase");
}

///////////////////////////////////////////////////////////////////////
//
// Slot0 members
//
///////////////////////////////////////////////////////////////////////

Util::Slot0::Slot0()
{
DOTRACE("Util::Slot0::Slot0");
}

Util::Slot0::~Slot0()
{
DOTRACE("Util::Slot0::~Slot0");
}

///////////////////////////////////////////////////////////////////////
//
// SignalBase::Impl class
//
///////////////////////////////////////////////////////////////////////

namespace
{
  typedef Util::Ref<Util::SlotBase> SlotRef;
}

struct Util::SignalBase::Impl
{
public:
  Impl() :
    slots(),
    isItEmitting(false)
  {}

  virtual ~Impl() {}

  typedef dlink_list<SlotRef> ListType;

  ListType slots;
  bool isItEmitting;

  class Lock
  {
    Lock(const Lock&);
    Lock& operator=(const Lock&);

    Impl* itsImpl;

  public:
    Lock(Impl* impl) :
      itsImpl(impl)
    {
      Assert(itsImpl->isItEmitting == false);
      itsImpl->isItEmitting = true;
    }

    ~Lock()
    {
      Assert(itsImpl->isItEmitting == true);
      itsImpl->isItEmitting = false;
    }
  };
};

Util::SignalBase::SignalBase() :
  Util::VolatileObject(),
  rep(new Impl)
{}

Util::SignalBase::~SignalBase()
{
  delete rep;
}

void Util::SignalBase::doEmit(void* params) const
{
DOTRACE("Util::SignalBase::doEmit");
  if (!rep->isItEmitting)
    {
      Impl::Lock lock(rep);

      for (Impl::ListType::iterator
             ii = rep->slots.begin(), end = rep->slots.end();
           ii != end;
           /* incr in loop */)
        {
          dbgEval(3, typeid(**ii).name());
          dbgEvalNL(3, (*ii)->refCount());
          if ((*ii)->exists())
            {
              (*ii)->doCall(params);
              ++ii;
            }
          else
            {
              Impl::ListType::iterator erase_me = ii;
              ++ii;
              rep->slots.erase(erase_me);
            }
        }
    }
}

void Util::SignalBase::doDisconnect(Util::SoftRef<Util::SlotBase> slot)
{
DOTRACE("Util::SignalBase::doDisconnect");
  if (!slot.isValid()) return;

  rep->slots.remove(SlotRef(slot.get(), Util::PRIVATE));

  dbgEvalNL(3, rep->slots.size());
}

void Util::SignalBase::doConnect(Util::SoftRef<Util::SlotBase> slot)
{
DOTRACE("Util::SignalBase::doConnect");
  if (!slot.isValid()) return;

  rep->slots.push_back(SlotRef(slot.get(), Util::PRIVATE));

  dbgEvalNL(3, rep->slots.size());
}

///////////////////////////////////////////////////////////////////////
//
// Signal0 method definitions
//
///////////////////////////////////////////////////////////////////////

Util::Signal0::Signal0() :
  SignalBase(),
  slotEmitSelf(Slot0::make(this, &Util::Signal0::emit))
{
DOTRACE("Util::Signal0::Signal0");
}

Util::Signal0::~Signal0()
{
DOTRACE("Util::Signal0::~Signal0");
}

static const char vcid_signal_cc[] = "$Header$";
#endif // !SIGNAL_CC_DEFINED
