///////////////////////////////////////////////////////////////////////
//
// signal.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue May 25 18:39:27 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef SIGNAL_CC_DEFINED
#define SIGNAL_CC_DEFINED

#include "util/signal.h"

#include "util/demangle.h"
#include "util/dlink_list.h"
#include "util/ref.h"

#include <typeinfo>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER;

///////////////////////////////////////////////////////////////////////
//
// SlotBase members
//
///////////////////////////////////////////////////////////////////////

Util::SlotBase::SlotBase()
{
DOTRACE("Util::SlotBase::SlotBase");
}

Util::SlotBase::~SlotBase() throw()
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

Util::Slot0::~Slot0() throw()
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

    Impl* itsTarget;

  public:
    Lock(Impl* impl) :
      itsTarget(impl)
    {
      Assert(itsTarget->isItEmitting == false);
      itsTarget->isItEmitting = true;
    }

    ~Lock()
    {
      Assert(itsTarget->isItEmitting == true);
      itsTarget->isItEmitting = false;
    }
  };
};

Util::SignalBase::SignalBase() :
  Util::VolatileObject(),
  rep(new Impl)
{}

Util::SignalBase::~SignalBase() throw()
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
          dbgEvalNL(3, (*ii)->dbg_RefCount());
          dbgEvalNL(3, (*ii)->exists());
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

Util::Signal0::~Signal0() throw()
{
DOTRACE("Util::Signal0::~Signal0");
}

static const char vcid_signal_cc[] = "$Header$";
#endif // !SIGNAL_CC_DEFINED
