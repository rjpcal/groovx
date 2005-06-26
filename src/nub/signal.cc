///////////////////////////////////////////////////////////////////////
//
// signal.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue May 25 18:39:27 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_NUB_SIGNAL_CC_UTC20050626084019_DEFINED
#define GROOVX_NUB_SIGNAL_CC_UTC20050626084019_DEFINED

#include "signal.h"

#include "nub/ref.h"

#include "rutz/demangle.h"

#include <list>
#include <typeinfo>

#include "rutz/trace.h"
#include "rutz/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// SlotBase members
//
///////////////////////////////////////////////////////////////////////

Nub::SlotBase::SlotBase()
{
DOTRACE("Nub::SlotBase::SlotBase");
}

Nub::SlotBase::~SlotBase() throw()
{
DOTRACE("Nub::SlotBase::~SlotBase");
}

bool Nub::SlotBase::exists() const
{
DOTRACE("Nub::SlotBase::exists");
  return true;
}

///////////////////////////////////////////////////////////////////////
//
// Slot0 members
//
///////////////////////////////////////////////////////////////////////

Nub::Slot0::Slot0()
{
DOTRACE("Nub::Slot0::Slot0");
}

Nub::Slot0::~Slot0() throw()
{
DOTRACE("Nub::Slot0::~Slot0");
}

Nub::SoftRef<Nub::Slot0> Nub::Slot0::make(void (*freeFunc)())
{
DOTRACE("Nub::Slot0::make");

  return Nub::SoftRef<Slot0>(SlotAdapterFreeFunc0::make(freeFunc));
}

Nub::SlotAdapterFreeFunc0::SlotAdapterFreeFunc0(FreeFunc* f)
  : itsFreeFunc(f)
{}

Nub::SlotAdapterFreeFunc0::~SlotAdapterFreeFunc0() throw() {}

Nub::SlotAdapterFreeFunc0*
Nub::SlotAdapterFreeFunc0::make(FreeFunc* f)
{
  return new SlotAdapterFreeFunc0(f);
}

void Nub::SlotAdapterFreeFunc0::call()
{
  (*itsFreeFunc)();
}

///////////////////////////////////////////////////////////////////////
//
// SignalBase::Impl class
//
///////////////////////////////////////////////////////////////////////

namespace
{
  typedef Nub::Ref<Nub::SlotBase> SlotRef;
}

struct Nub::SignalBase::Impl
{
public:
  Impl() :
    slots(),
    isItEmitting(false)
  {}

  virtual ~Impl() {}

  typedef std::list<SlotRef> ListType;

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
      ASSERT(itsTarget->isItEmitting == false);
      itsTarget->isItEmitting = true;
    }

    ~Lock()
    {
      ASSERT(itsTarget->isItEmitting == true);
      itsTarget->isItEmitting = false;
    }
  };
};

Nub::SignalBase::SignalBase() :
  Nub::VolatileObject(),
  rep(new Impl)
{}

Nub::SignalBase::~SignalBase() throw()
{
  delete rep;
}

void Nub::SignalBase::doEmit(void* params) const
{
DOTRACE("Nub::SignalBase::doEmit");
  if (!rep->isItEmitting)
    {
      Impl::Lock lock(rep);

      for (Impl::ListType::iterator
             ii = rep->slots.begin(), end = rep->slots.end();
           ii != end;
           /* incr in loop */)
        {
          dbg_eval(3, typeid(**ii).name());
          dbg_eval_nl(3, (*ii)->dbg_RefCount());
          dbg_eval_nl(3, (*ii)->exists());
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

void Nub::SignalBase::doDisconnect(Nub::SoftRef<Nub::SlotBase> slot)
{
DOTRACE("Nub::SignalBase::doDisconnect");
  if (!slot.isValid()) return;

  rep->slots.remove(SlotRef(slot.get(), Nub::PRIVATE));

  dbg_eval_nl(3, rep->slots.size());
}

void Nub::SignalBase::doConnect(Nub::SoftRef<Nub::SlotBase> slot)
{
DOTRACE("Nub::SignalBase::doConnect");
  if (!slot.isValid()) return;

  rep->slots.push_back(SlotRef(slot.get(), Nub::PRIVATE));

  dbg_eval_nl(3, rep->slots.size());
}

///////////////////////////////////////////////////////////////////////
//
// Signal0 method definitions
//
///////////////////////////////////////////////////////////////////////

Nub::Signal0::Signal0() :
  SignalBase(),
  slotEmitSelf(Slot0::make(this, &Nub::Signal0::emit))
{
DOTRACE("Nub::Signal0::Signal0");
}

Nub::Signal0::~Signal0() throw()
{
DOTRACE("Nub::Signal0::~Signal0");
}

static const char vcid_groovx_nub_signal_cc_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_SIGNAL_CC_UTC20050626084019_DEFINED
