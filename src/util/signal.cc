///////////////////////////////////////////////////////////////////////
//
// signal.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:39:27 1999
// written: Mon Nov 25 12:32:54 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIGNAL_CC_DEFINED
#define SIGNAL_CC_DEFINED

#include "util/signal.h"

#include "util/dlink_list.h"
#include "util/ref.h"

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
    itsSlots(),
    isItEmitting(false)
  {}

  virtual ~Impl() {}

  typedef dlink_list<SlotRef> ListType;

  ListType itsSlots;

  bool isItEmitting;

  void emit(void* params)
  {
    if (!isItEmitting)
      {
        Lock lock(this);

        for (ListType::iterator ii = itsSlots.begin(), end = itsSlots.end();
             ii != end;
             /* incr in loop */)
          {
            if ((*ii)->exists())
              {
                dbgEval(3, (*ii)->refCount());
                (*ii)->doCall(params);
                ++ii;
              }
            else
              {
                ListType::iterator erase_me = ii;
                ++ii;
                itsSlots.erase(erase_me);
              }
          }
      }
  }

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
DOTRACE("Util::SignalBase::emit");
  rep->emit(params);
}

void Util::SignalBase::disconnect(Util::SoftRef<Util::SlotBase> slot)
{
DOTRACE("Util::SignalBase::disconnect");
  if (!slot.isValid()) return;

  rep->itsSlots.remove(SlotRef(slot.get(), Util::PRIVATE));

  dbgEvalNL(3, rep->itsSlots.size());
}

void Util::SignalBase::connect(Util::SoftRef<Util::SlotBase> slot)
{
DOTRACE("Util::SignalBase::connect");
  if (!slot.isValid()) return;

  rep->itsSlots.push_back(SlotRef(slot.get(), Util::PRIVATE));

  dbgEvalNL(3, rep->itsSlots.size());
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

void Util::Signal0::disconnect(Util::SoftRef<Util::Slot0> slot)
{
  SignalBase::disconnect(slot);
}

void Util::Signal0::connect(Util::SoftRef<Util::Slot0> slot)
{
  SignalBase::connect(slot);
}

static const char vcid_signal_cc[] = "$Header$";
#endif // !SIGNAL_CC_DEFINED
