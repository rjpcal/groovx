///////////////////////////////////////////////////////////////////////
//
// signal.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:39:27 1999
// written: Mon Nov 25 11:05:21 2002
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
// Slot members
//
///////////////////////////////////////////////////////////////////////

Util::Slot::Slot()
{
DOTRACE("Util::Slot::Slot");
}

Util::Slot::~Slot()
{
DOTRACE("Util::Slot::~Slot");
}

///////////////////////////////////////////////////////////////////////
//
// SignalBase::Impl class
//
///////////////////////////////////////////////////////////////////////

namespace
{
  typedef Util::Ref<Util::Slot> SlotRef;
}

struct Util::SignalBase::Impl
{
public:
  Impl(SignalBase* owner) :
    itsSlots(),
    slotEmitSelf(Util::Slot::make(owner, &SignalBase::receive)),
    isItEmitting(false)
  {}

  virtual ~Impl() {}

  typedef dlink_list<SlotRef> ListType;

  ListType itsSlots;

  Util::Ref<Util::Slot> slotEmitSelf;

  bool isItEmitting;

  void emit()
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
                (*ii)->call();
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
  rep(new Impl(this))
{}

Util::SignalBase::~SignalBase()
{
  delete rep;
}

void Util::SignalBase::receive() { rep->emit(); }
void Util::SignalBase::emit() const { rep->emit(); }

void Util::SignalBase::disconnect(Util::SoftRef<Util::Slot> slot)
{
DOTRACE("Util::SignalBase::disconnect");
  if (!slot.isValid()) return;

  rep->itsSlots.remove(SlotRef(slot.get(), Util::PRIVATE));

  dbgEvalNL(3, rep->itsSlots.size());
}

void Util::SignalBase::connect(Util::SoftRef<Util::Slot> slot)
{
DOTRACE("Util::SignalBase::connect");
  if (!slot.isValid()) return;

  rep->itsSlots.push_back(SlotRef(slot.get(), Util::PRIVATE));

  dbgEvalNL(3, rep->itsSlots.size());
}

Util::SoftRef<Util::Slot> Util::SignalBase::slot() const
{
DOTRACE("Util::SignalBase::slot");
  return rep->slotEmitSelf;
}

///////////////////////////////////////////////////////////////////////
//
// Signal method definitions
//
///////////////////////////////////////////////////////////////////////

Util::Signal::Signal() :
  SignalBase()
{
DOTRACE("Util::Signal::Signal");
}

Util::Signal::~Signal()
{
DOTRACE("Util::Signal::~Signal");
}

void Util::Signal::disconnect(Util::SoftRef<Util::Slot> slot)
{
  SignalBase::disconnect(slot);
}

void Util::Signal::connect(Util::SoftRef<Util::Slot> slot)
{
  SignalBase::connect(slot);
}

void Util::Signal::emit() const
{
  SignalBase::emit();
}

Util::SoftRef<Util::Slot> Util::Signal::slot() const
{
  return SignalBase::slot();
}

static const char vcid_signal_cc[] = "$Header$";
#endif // !SIGNAL_CC_DEFINED
