/** @file nub/signal.cc a template-based signal/slot mechanism, similar
    to that of libsigc++ */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue May 25 18:39:27 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#include "signal.h"

#include "nub/ref.h"

#include "rutz/demangle.h"

#include <typeinfo>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// slot_base members
//
///////////////////////////////////////////////////////////////////////

nub::slot_base::slot_base()
{
GVX_TRACE("nub::slot_base::slot_base");
}

nub::slot_base::~slot_base() noexcept
{
GVX_TRACE("nub::slot_base::~slot_base");
}

///////////////////////////////////////////////////////////////////////
//
// slot0 members
//
///////////////////////////////////////////////////////////////////////

nub::slot0::slot0()
{
GVX_TRACE("nub::slot0::slot0");
}

nub::slot0::~slot0() noexcept
{
GVX_TRACE("nub::slot0::~slot0");
}

nub::soft_ref<nub::slot0> nub::slot0::make(void (*free_func)())
{
GVX_TRACE("nub::slot0::make");

  return nub::soft_ref<slot0>(slot_adapter_free_func0::make(free_func));
}

nub::slot_adapter_free_func0::slot_adapter_free_func0(free_func* f)
  : m_free_func(f)
{}

nub::slot_adapter_free_func0::~slot_adapter_free_func0() noexcept {}

nub::slot_adapter_free_func0*
nub::slot_adapter_free_func0::make(free_func* f)
{
  return new slot_adapter_free_func0(f);
}

void nub::slot_adapter_free_func0::call()
{
  (*m_free_func)();
}

///////////////////////////////////////////////////////////////////////
//
// signal_base::impl class
//
///////////////////////////////////////////////////////////////////////

nub::signal_base::signal_base() :
  nub::volatile_object(),
  slots(),
  is_emitting(false)
{}

nub::signal_base::~signal_base() noexcept
{}

void nub::signal_base::do_emit(void* params) const
{
GVX_TRACE("nub::signal_base::do_emit");
  if (!this->is_emitting)
    {
      locker lock(this);

      for (list_type::iterator
             ii = this->slots.begin(), end = this->slots.end();
           ii != end;
           /* incr in loop */)
        {
          if (GVX_DBG_LEVEL() >= 3)
            {
              const nub::slot_base& thing = *(ii->sref);
              dbg_eval(3, typeid(thing).name());
              dbg_eval_nl(3, ii->sref->dbg_ref_count());
              dbg_eval_nl(3, ii->all_valid());
            }
          if (ii->all_valid())
            {
              ii->sref->do_call(params);
              ++ii;
            }
          else
            {
              list_type::iterator erase_me = ii;
              ++ii;
              this->slots.erase(erase_me);
            }
        }
    }
}

void nub::signal_base::do_disconnect(nub::soft_ref<nub::slot_base> slot)
{
GVX_TRACE("nub::signal_base::do_disconnect");
  if (!slot.is_valid()) return;

  this->slots.remove_if([id=slot.id()](const slot_info& si){ return si.sref.id() == id; });

  dbg_eval_nl(3, this->slots.size());
}

void nub::signal_base::do_connect(nub::soft_ref<nub::slot_base> slot,
                                  nub::soft_ref<nub::object> trackme)
{
GVX_TRACE("nub::signal_base::do_connect");
  if (!slot.is_valid()) return;

  this->slots.push_back(slot_info({slot_ref(slot.get(), nub::ref_vis::PRIVATE), {}}));
  if (trackme.is_valid())
    this->slots.back().tracked.push_front(std::move(trackme));
  dbg_eval_nl(3, this->slots.size());
}

///////////////////////////////////////////////////////////////////////
//
// signal0 method definitions
//
///////////////////////////////////////////////////////////////////////

nub::signal0::signal0() :
  signal_base(),
  slot_emit_self(slot0::make(this, &nub::signal0::emit))
{
GVX_TRACE("nub::signal0::signal0");
}

nub::signal0::~signal0() noexcept
{
GVX_TRACE("nub::signal0::~signal0");
}
