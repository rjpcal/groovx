///////////////////////////////////////////////////////////////////////
//
// signal.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:39:27 1999
// written: Sun Nov  3 13:41:11 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIGNAL_CC_DEFINED
#define SIGNAL_CC_DEFINED

#include "util/signal.h"

#include "util/dlink_list.h"
#include "util/ref.h"
#include "util/volatileobject.h"

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
// Signal::SigImpl class
//
///////////////////////////////////////////////////////////////////////

namespace
{
  typedef Util::Ref<Util::Slot> SlotRef;
}

struct Util::Signal::SigImpl : public Util::VolatileObject
{
public:

  SigImpl() :
    itsSlots(),
    slotEmitSelf(Util::Slot::make(this, & SigImpl::receive)),
    isItEmitting(false)
  {}

  virtual ~SigImpl() {}

  static SigImpl* make() { return new SigImpl; }

  typedef dlink_list<SlotRef> ListType;

  ListType itsSlots;

  Util::Ref<Util::Slot> slotEmitSelf;

  bool isItEmitting;

  void receive()
  {
    emit();
  }

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

    SigImpl* itsImpl;

  public:
    Lock(SigImpl* impl) :
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

///////////////////////////////////////////////////////////////////////
//
// Signal method definitions
//
///////////////////////////////////////////////////////////////////////

Util::Signal::Signal() :
  itsImpl(SigImpl::make())
{
DOTRACE("Util::Signal::Signal");
}

Util::Signal::~Signal()
{
DOTRACE("Util::Signal::~Signal");
  itsImpl->destroy();
}

void Util::Signal::disconnect(Util::SoftRef<Util::Slot> slot)
{
DOTRACE("Util::Signal::disconnect");
  if (!slot.isValid()) return;

  itsImpl->itsSlots.remove(SlotRef(slot.get(), Util::PRIVATE));

  dbgEvalNL(3, itsImpl->itsSlots.size());
}

void Util::Signal::connect(Util::SoftRef<Util::Slot> slot)
{
DOTRACE("Util::Signal::connect");
  if (!slot.isValid()) return;

  itsImpl->itsSlots.push_back(SlotRef(slot.get(), Util::PRIVATE));

  dbgEvalNL(3, itsImpl->itsSlots.size());
}

void Util::Signal::emit() const
{
  itsImpl->emit();
}

Util::SoftRef<Util::Slot> Util::Signal::slot() const
{
DOTRACE("Util::Signal::slot");
  return itsImpl->slotEmitSelf;
}

static const char vcid_signal_cc[] = "$Header$";
#endif // !SIGNAL_CC_DEFINED
