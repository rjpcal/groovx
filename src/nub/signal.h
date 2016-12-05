/** @file nub/signal.h a template-based signal/slot mechanism, similar
    to that of libsigc++ */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue May 25 18:29:04 1999
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

#ifndef GROOVX_NUB_SIGNAL_H_UTC20050626084019_DEFINED
#define GROOVX_NUB_SIGNAL_H_UTC20050626084019_DEFINED

#include "nub/object.h"
#include "nub/ref.h"
#include "nub/volatileobject.h"

#include <algorithm>
#include <forward_list>
#include <functional>
#include <list>

namespace nub
{
#if 0
  template <class R>
  class marshal
  {
    R val;

  public:
    marshal() : val() {}

    typedef R in_type;
    typedef R out_type;

    bool marshal(const in_type& in)
    {
      val = in;
    }

    out_type value() const { return val; }
  };
#endif


  //  #######################################################
  //  =======================================================

  /// signal - generic implementation of the observer pattern

  /** Classes that need to notify others of changes should hold an
      appropriate signal object by value, and call emit() when it is
      necessary to notify observers of the change. In turn, emit()
      will \c call all of the callbacks that are observing this
      signal. */

  template <class... Args>
  class signal : public nub::volatile_object
  {
  public:
    typedef std::function<void(Args...)> func_type;

  private:

    class emit_locker
    {
      const signal* m_target;

    public:
      emit_locker(const signal* impl) : m_target(impl)
      { m_target->is_emitting = true; }

      ~emit_locker()
      { m_target->is_emitting = false; }

      emit_locker(const emit_locker&) = delete;
      emit_locker& operator=(const emit_locker&) = delete;
    };

    struct slot_info
    {
      func_type func;
      std::forward_list<nub::soft_ref<const nub::object>> tracked;

      slot_info(func_type func_, const nub::object* trackme)
        : func(std::move(func_))
      {
        if (trackme)
          tracked.emplace_front(trackme, nub::ref_type::WEAK, nub::ref_vis_private());
      }
      bool is_tracking(const nub::object* t) const
      {
        return std::any_of(tracked.begin(), tracked.end(),
                           [t](const auto& it){return it.get_weak() == t;});
      }
      bool all_valid() const
      {
        return std::all_of(tracked.begin(), tracked.end(),
                           [](const auto& it){return it.is_valid();});
      }
    };

    typedef std::list<slot_info> list_type;

    mutable list_type slots;
    mutable bool is_emitting;

  public:
    signal() :
      nub::volatile_object(),
      slots(),
      is_emitting(false)
    {}

    virtual ~signal() noexcept = default;

    signal(const signal&) = delete;
    signal& operator=(const signal&) = delete;

    void emit(Args... args) const
    {
      if (this->is_emitting)
        return;

      emit_locker lock(this);

      for (typename list_type::iterator
             ii = this->slots.begin(), end = this->slots.end();
           ii != end;
           /* incr in loop */)
        {
          if (ii->all_valid())
            {
              ii->func(std::forward<Args>(args)...);
              ++ii;
            }
          else
            {
              typename list_type::iterator erase_me = ii;
              ++ii;
              this->slots.erase(erase_me);
            }
        }
    }

    /// Opaque handle type returned by connect() functions; can be passed later to disconnect().
    struct connection
    {
      connection(const slot_info* i = nullptr) : info(i) {}
      const slot_info* info;
    };

    /// Add a function callback to the list of observers
    /** If trackme is set, then also bind that callback's lifetime to
        trackme's lifetime */
    connection connect(func_type func, const nub::object* trackme = nullptr)
    {
      if (!func) return connection{nullptr};

      this->slots.emplace_back(std::move(func), trackme);

      return connection{&(this->slots.back())};
    }

    /// Connect an object to this signal.
    /** After connection, when the signal is triggered, \a mem_func
        will be called on \a obj. */
    template <class C, class MF>
    connection connect(C* obj, MF mem_func)
    {
      return this->connect(func_type([obj,mem_func](Args... args){
            ((*obj).*mem_func)(std::forward<Args>(args)...);
          }), obj);
    }

    /// Connect another signal to this.
    /** When this signal emit()s, other's emit() will also be triggered. */
    connection connect(signal* other)
    { return connect(other, &signal::emit); }

    /// Remove a slot from the list of observers
    void disconnect(connection c)
    {
      if (c.info)
        this->slots.remove_if([t=c.info](const slot_info& si){ return &si == t; });
    }

    /// Remove slots w/ given tracked object from the list of observers
    void disconnect(const nub::object* tracked)
    {
      if (tracked)
        this->slots.remove_if([t=tracked](const slot_info& si){ return si.is_tracking(t); });
    }

  };

} // end namespace nub

#endif // !GROOVX_NUB_SIGNAL_H_UTC20050626084019_DEFINED
