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

  /// signal_base provides basic implementation for Signal.

  /** Classes that need to notify others of changes should hold an
      appropriate signal object by value, and call emit() when it is
      necessary to notify observers of the change. In turn, emit()
      will \c call all of the slots that are observing this
      Signal. */

  template <class... Args>
  class signal_base : public nub::volatile_object
  {
  public:
    typedef std::function<void(Args...)> func_type;

  private:

    struct slot_info
    {
      func_type func;
      std::forward_list<nub::soft_ref<const nub::object>> tracked;
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

  protected:
    signal_base() :
      nub::volatile_object(),
      slots(),
      is_emitting(false)
    {}

    virtual ~signal_base() noexcept = default;

    void do_emit(Args... args) const
    {
      if (this->is_emitting)
        return;

      locker lock(this);

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

  public:

    struct connection
    {
      connection(const slot_info* i = nullptr) : info(i) {}
      const slot_info* info;
    };

  protected:
    /// Add a slot to the list of those watching this Signal, and bind that slot's lifetime to a tracked object's lifetime
    connection connect(func_type func, const nub::object* trackme)
    {
      if (!func) return connection{nullptr};

      this->slots.push_back(slot_info({func,
              {nub::soft_ref<const nub::object>(trackme, nub::ref_type::WEAK, nub::ref_vis_private())}}));

      return connection{&(this->slots.back())};
    }

    /// Add a slot to the list of those watching this Signal
    connection connect(func_type func)
    {
      if (!func) return connection{nullptr};

      this->slots.push_back(slot_info({func, {}}));
      return connection{&(this->slots.back())};
    }

  public:
    /// Remove a slot from the list of those watching this Signal.
    void disconnect(connection c)
    {
      if (c.info)
        this->slots.remove_if([t=c.info](const slot_info& si){ return &si == t; });
    }

    /// Remove slots w/ given tracked object from the list of those watching this Signal.
    void disconnect(const nub::object* tracked)
    {
      if (tracked)
        this->slots.remove_if([t=tracked](const slot_info& si){ return si.is_tracking(t); });
    }

  private:
    signal_base(const signal_base&);
    signal_base& operator=(const signal_base&);

    class locker
    {
      locker(const locker&);
      locker& operator=(const locker&);

      const signal_base* m_target;

    public:
      locker(const signal_base* impl) : m_target(impl)
      { m_target->is_emitting = true; }

      ~locker()
      { m_target->is_emitting = false; }
    };
  };


  //  #######################################################
  //  =======================================================

  /// A zero-argument signal.

  class signal0 : public signal_base<>
  {
  public:
    /// Default constructor.
    signal0() = default;

    /// Virtual destructor.
    virtual ~signal0() noexcept = default;

    using typename signal_base<>::func_type;
    using typename signal_base<>::connection;

    /// Connect a free function to this signal0.
    connection connect(void (*free_func)())
    { return signal_base<>::connect(func_type(free_func)); }

    /// Connect an object to this signal0.
    /** After connection, when the signal is triggered, \a mem_func
        will be called on \a obj. \c connect() returns the nub::uid of
        the connection object that is created. */
    template <class C, class MF>
    connection connect(C* obj, MF mem_func)
    { return signal_base<>::connect(func_type([obj,mem_func](){((*obj).*mem_func)();}), obj); }

    connection connect(signal0* other)
    { return connect(other, &signal0::emit); }

    /// Trigger all of this object's slots.
    void emit() const { signal_base<>::do_emit(); }

  private:
    signal0(const signal0&);
    signal0& operator=(const signal0&);
  };


  //  #######################################################
  //  =======================================================

  /// A one-argument signal.

  template <class P1>
  class Signal1 : public signal_base<P1>
  {
  public:
    Signal1() {}

    virtual ~Signal1() noexcept {}

    using typename signal_base<P1>::connection;
    using typename signal_base<P1>::func_type;

    connection connect(void (*free_func)(P1))
    { return signal_base<P1>::connect(func_type(free_func)); }

    template <class C, class MF>
    connection connect(C* obj, MF mem_func)
    { return signal_base<P1>::connect(func_type([obj,mem_func](P1 p1){((*obj).*mem_func)(p1);}), obj); }

    void emit(P1 p1) const
    {
      signal_base<P1>::do_emit(p1);
    }

  private:
    Signal1(const Signal1&);
    Signal1& operator=(const Signal1&);
  };

} // end namespace nub

#endif // !GROOVX_NUB_SIGNAL_H_UTC20050626084019_DEFINED
