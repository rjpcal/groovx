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

  /// The base class for all slot classes.

  /** Slots can be triggered from a Signal by calling connect() on
      that Signal. Thereafter, the slot will be called whenever that
      signal emits a message. */

  class slot_base : public virtual nub::object
  {
  public:
    /// Default constructor.
    slot_base();

    /// Virtual destructor.
    virtual ~slot_base() noexcept;

    /// Call the slot with the given arguments
    virtual void do_call(void* params) = 0;
  };

  //  #######################################################
  //  =======================================================

  /// A zero-argument slot class.

  class slot0 : public slot_base
  {
  public:
    /// Default constructor.
    slot0();

    /// Virtual destructor.
    virtual ~slot0() noexcept;

    template <class C, class MF>
    static nub::soft_ref<slot0> make(C* obj, MF mf);

    static nub::soft_ref<slot0> make(void (*free_func)());

    virtual void call() = 0;

    /// Unpacks any parameters and then calls the implementation.
    virtual void do_call(void* /*params*/) override { call(); }
  };


  //  #######################################################
  //  =======================================================

  /// A mem-func adapter for zero-argument slots.

  template <class C, class MF>
  class slot_adapter_mem_func0 : public slot0
  {
    nub::soft_ref<C> m_object;
    MF m_mem_func;

    slot_adapter_mem_func0(C* obj, MF mf) :
      m_object(obj, nub::ref_type::WEAK, nub::ref_vis_private()), m_mem_func(mf) {}

    virtual ~slot_adapter_mem_func0() noexcept {}

  public:
    static slot_adapter_mem_func0<C, MF>* make(C* obj, MF mf)
    { return new slot_adapter_mem_func0<C, MF>(obj, mf); }

    virtual void call() override
    {
      if (m_object.is_valid())
        (m_object.get()->*m_mem_func)();
    }
  };

  template <class C, class MF>
  inline nub::soft_ref<slot0> slot0::make(C* obj, MF mf)
  {
    return nub::soft_ref<slot0>
      (slot_adapter_mem_func0<C, MF>::make(obj, mf),
       nub::ref_type::STRONG,
       nub::ref_vis_private());
  }

  //  #######################################################
  //  =======================================================

  /// A mem-func adapter for zero-argument slots.

  class slot_adapter_free_func0 : public slot0
  {
  public:
    typedef void (free_func)();

  private:
    free_func* m_free_func;

    slot_adapter_free_func0(free_func* f);

    virtual ~slot_adapter_free_func0() noexcept;

  public:
    static slot_adapter_free_func0* make(free_func* f);

    virtual void call() override;
  };

  //  #######################################################
  //  =======================================================

  /// A one-argument call data wrapper.

  template <class P1>
  struct call_data1
  {
    call_data1(P1 i1) : p1(i1) {}
    P1 p1;
  };

  //  #######################################################
  //  =======================================================

  /// A slot with one argument.

  template <class P1>
  class slot1 : public slot_base
  {
  public:
    /// Default constructor.
    slot1() {}

    /// Virtual destructor.
    virtual ~slot1() noexcept {}

    template <class C, class MF>
    static nub::soft_ref<slot1<P1> > make(C* obj, MF mf);

    template <class FF>
    static nub::soft_ref<slot1<P1> > make(FF f);

    virtual void call(P1 p1) = 0;

    /// Unpacks any parameters and then calls the implementation.
    virtual void do_call(void* params) override
    {
      call_data1<P1>* data = static_cast<call_data1<P1>*>(params);
      call(data->p1);
    }
  };

  //  #######################################################
  //  =======================================================

  /// A mem-func adapter for slots with one argument.

  template <class P1, class C, class MF>
  class slot_adapter_mem_func1 : public slot1<P1>
  {
    nub::soft_ref<C> m_object;
    MF m_mem_func;

    slot_adapter_mem_func1(C* obj, MF mf) :
      m_object(obj, nub::ref_type::WEAK, nub::ref_vis_private()), m_mem_func(mf) {}

    virtual ~slot_adapter_mem_func1() noexcept {}

  public:
    static slot_adapter_mem_func1<P1, C, MF>* make(C* obj, MF mf)
    { return new slot_adapter_mem_func1<P1, C, MF>(obj, mf); }

    virtual void call(P1 p1) override
    {
      if (m_object.is_valid())
        (m_object.get()->*m_mem_func)(p1);
    }
  };

  template <class P1>
  template <class C, class MF>
  inline nub::soft_ref<slot1<P1> > slot1<P1>::make(C* obj, MF mf)
  {
    return nub::soft_ref<slot1<P1> >
      (slot_adapter_mem_func1<P1, C, MF>::make(obj, mf),
       nub::ref_type::STRONG,
       nub::ref_vis_private());
  }


  //  #######################################################
  //  =======================================================

  /// A free-func adapter for slots with one argument.

  template <class P1, class FF>
  class slot_adapter_free_func1 : public slot1<P1>
  {
    FF m_free_func;

    slot_adapter_free_func1(FF f) : m_free_func(f) {}

    virtual ~slot_adapter_free_func1() noexcept {}

  public:
    static slot_adapter_free_func1<P1, FF>* make(FF f)
    { return new slot_adapter_free_func1<P1, FF>(f); }

    virtual void call(P1 p1) override
    {
      (*m_free_func)(p1);
    }
  };

  template <class P1>
  template <class FF>
  inline nub::soft_ref<slot1<P1> > slot1<P1>::make(FF f)
  {
    return nub::soft_ref<slot1<P1> >
      (slot_adapter_free_func1<P1, FF>::make(f),
       nub::ref_type::STRONG,
       nub::ref_vis_private());
  }


  //  #######################################################
  //  =======================================================

  /// signal_base provides basic implementation for Signal.

  /** Classes that need to notify others of changes should hold an
      appropriate signal object by value, and call emit() when it is
      necessary to notify observers of the change. In turn, emit()
      will \c call all of the slot's that are observing this
      Signal. */

  class signal_base : public nub::volatile_object
  {
  private:
    typedef nub::ref<nub::slot_base> slot_ref;

    struct slot_info
    {
      slot_ref sref;
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

    void do_emit(void* params) const
    {
      if (this->is_emitting)
        return;

      locker lock(this);

      for (list_type::iterator
             ii = this->slots.begin(), end = this->slots.end();
           ii != end;
           /* incr in loop */)
        {
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

  public:

    struct connection
    {
      connection(const slot_info* i = nullptr) : info(i) {}
      const slot_info* info;
    };

  protected:
    /// Add a slot to the list of those watching this Signal, and bind that slot's lifetime to a tracked object's lifetime
    connection connect(nub::soft_ref<slot_base> slot, const nub::object* trackme)
    {
      if (!slot.is_valid()) return connection{nullptr};

      this->slots.push_back(slot_info({slot_ref(slot.get(), nub::ref_vis_private()),
              {nub::soft_ref<const nub::object>(trackme, nub::ref_type::WEAK, nub::ref_vis_private())}}));

      return connection{&(this->slots.back())};
    }

    /// Add a slot to the list of those watching this Signal
    connection connect(nub::soft_ref<slot_base> slot)
    {
      if (!slot.is_valid()) return connection{nullptr};

      this->slots.push_back(slot_info({slot_ref(slot.get(), nub::ref_vis_private()), {}}));
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

  class signal0 : public signal_base
  {
  public:
    /// Default constructor.
    signal0();

    /// Virtual destructor.
    virtual ~signal0() noexcept;

    using signal_base::connection;

    /// Connect a free function to this signal0.
    connection connect(void (*free_func)())
    { return signal_base::connect(slot0::make(free_func)); }

    /// Connect an object to this signal0.
    /** After connection, when the signal is triggered, \a mem_func
        will be called on \a obj. \c connect() returns the nub::uid of
        the connection object that is created. */
    template <class C, class MF>
    connection connect(C* obj, MF mem_func)
    { return signal_base::connect(slot0::make(obj, mem_func), obj); }

    connection connect(signal0* other)
    { return connect(other, &signal0::emit); }

    /// Trigger all of this object's slots.
    void emit() const { signal_base::do_emit(nullptr); }

  private:
    signal0(const signal0&);
    signal0& operator=(const signal0&);
  };


  //  #######################################################
  //  =======================================================

  /// A one-argument signal.

  template <class P1>
  class Signal1 : public signal_base
  {
  public:
    Signal1() {}

    virtual ~Signal1() noexcept {}

    using signal_base::connection;

    connection connect(void (*free_func)(P1))
    { return signal_base::connect(slot1<P1>::make(free_func)); }

    template <class C, class MF>
    connection connect(C* obj, MF mem_func)
    { return signal_base::connect(slot1<P1>::make(obj, mem_func), obj); }

    void emit(P1 p1) const
    {
      call_data1<P1> dat( p1 );
      signal_base::do_emit(static_cast<void*>(&dat));
    }

  private:
    Signal1(const Signal1&);
    Signal1& operator=(const Signal1&);
  };

} // end namespace nub

#endif // !GROOVX_NUB_SIGNAL_H_UTC20050626084019_DEFINED
