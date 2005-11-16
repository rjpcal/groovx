/** @file nub/refdetail.h helper functions for nub::ref and
    nub::soft_ref */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Jun 30 13:29:00 2005 (split from nub/ref.h)
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

#ifndef GROOVX_NUB_REFDETAIL_H_UTC20050630202900_DEFINED
#define GROOVX_NUB_REFDETAIL_H_UTC20050630202900_DEFINED

#include "nub/object.h"

#include "rutz/algo.h"         // for rutz::swap2()
#include "rutz/fileposition.h" // for SRC_POS macro
#include "rutz/stderror.h"     // for rutz::throw_bad_cast()

#include <typeinfo>

namespace nub
{
  enum ref_type { WEAK, STRONG };

  enum ref_vis
    {
      DEFAULT,   //! equivalent to result of get_default_ref_vis()
      PRIVATE,   //! nub::objectdb gets no reference to the object
      PROTECTED, //! nub::objectdb gets a weak reference to the object
      PUBLIC     //! nub::objectdb gets a strong reference to the object
    };

  //! Get the current default visibility (will control ref_vis DEFAULT).
  ref_vis get_default_ref_vis();

  //! Set the current default visibility (will control ref_vis DEFAULT).
  void set_default_ref_vis(ref_vis vis);
}

///////////////////////////////////////////////////////////
//
// nub::detail namespace for helping to implement the ref classes
//
///////////////////////////////////////////////////////////

namespace nub { namespace detail {

  template <class T, class unref_policy> class handle;
  template <class T>                     class weak_handle;

  bool is_valid_uid(nub::uid id) throw();
  nub::object* get_checked_item(nub::uid id);

  void insert_item(nub::object* obj, ref_vis vis);

  void throw_ref_null(const std::type_info& info, const rutz::file_pos& pos);
  void throw_ref_unshareable(const std::type_info& msg, const rutz::file_pos& pos);
  void throw_soft_ref_invalid(const std::type_info& info, const rutz::file_pos& pos);

  template <class T>
  inline T* get_casted_item(nub::uid id)
  {
    nub::object* obj = get_checked_item(id);
    T* t = dynamic_cast<T*>(obj);
    if (t == 0)
      rutz::throw_bad_cast(typeid(T), typeid(nub::object), SRC_POS);
    return t;
  }

  template <>
  inline nub::object* get_casted_item<nub::object>(nub::uid id)
  { return get_checked_item(id); }

  /// Policy class which unrefs objects by decrementing their ref count.
  template <class T>
  struct default_unref_policy
  {
    static void unref(T* t) throw() { t->decr_ref_count(); }
  };

  /// Policy class which decrements ref count without deletion.
  template <class T>
  struct no_delete_unref_policy
  {
    static void unref(T* t) throw() { t->decr_ref_count_no_delete(); }
  };

}} // end namespace nub::detail::, end nub::


/// A shared implementation class for nub::floating_ref and nub::ref.
/** Note that the only operation that can throw is the
    constructor, which throws in case it is passed a null or
    unshareable object pointer .*/
template <class T, class unref_policy>
class nub::detail::handle
{
public:
  explicit handle(T* master) : m_master(master)
  {
    if (master == 0)
      throw_ref_null(typeid(T), SRC_POS);

    if (master->is_not_shareable())
      throw_ref_unshareable(typeid(T), SRC_POS);

    m_master->incr_ref_count();
  }

  ~handle() throw()
  { unref_policy::unref(m_master); }

  handle(const handle& other) throw()
    :
    m_master(other.m_master)
  {
    m_master->incr_ref_count();
  }

  handle& operator=(const handle& other) throw()
  {
    handle other_copy(other);
    this->swap(other_copy);
    return *this;
  }

  T* get() const throw()
  { return m_master; }

  bool operator==(const handle& other) const throw()
  { return m_master == other.m_master; }

private:
  void swap(handle& other) throw()
  {
    rutz::swap2(m_master, other.m_master);
  }

  T* m_master;

}; // end nub::detail::handle<T>



/// Internal helper class for soft_ref.
/** weak_handle manages memory etc., and provides one important
    guarantee: it will never return an invalid pointer from get()
    (an exception will be raised if this would fail). */
template <class T>
class nub::detail::weak_handle
{
private:
  static nub::ref_counts* get_counts(T* master, ref_type tp) throw()
  {
    return (master && (tp == WEAK || master->is_not_shareable())) ?
      master->get_counts() : 0;
  }

public:
  weak_handle(T* master, ref_type tp) throw()
    :
    m_master(master),
    m_counts(get_counts(master, tp))
  { this->acquire(); }

  ~weak_handle() throw()
  { this->release(); }

  weak_handle(const weak_handle& other) throw()
    :
    m_master(other.m_master),
    m_counts(other.m_counts)
  { this->acquire(); }

  weak_handle& operator=(const weak_handle& other) throw()
  {
    weak_handle other_copy(other);
    this->swap(other_copy);
    return *this;
  }

  bool is_valid() const throw()
  {
    if (m_counts == 0) // implies we are using strong ref's
      {
        return (m_master != 0);
      }

    // else... (m_counts != 0) implies we are using weak ref's

    return m_counts->is_owner_alive();
  }

  T* get()      const         { this->ensure_valid(); return m_master; }
  T* get_weak() const throw() { return this->is_valid() ? m_master : 0; }

  ref_type get_ref_type() const throw()
  {
    return (m_master && !m_counts) ? STRONG : WEAK;
  }

private:
  void acquire() const throw()
  {
    if (m_master)
      {
        if (m_counts) m_counts->acquire_weak();
        else          m_master->incr_ref_count();
      }
  }

  void release() const throw()
  {
    if (m_master)
      {
        if (m_counts) m_counts->release_weak();
        else          m_master->decr_ref_count();
      }

    m_counts = 0; m_master = 0;
  }

  void ensure_valid() const
  {
    if (!is_valid())
      nub::detail::throw_soft_ref_invalid(typeid(T), SRC_POS);
  }

  void swap(weak_handle& other) throw()
  {
    rutz::swap2(m_master, other.m_master);
    rutz::swap2(m_counts, other.m_counts);
  }

  // In order to avoid storing a separate bool indicating whether
  // we are using strong or weak ref's, we use the following
  // convention: if we are strong ref-counting, then m_counts
  // stays null, but if we are weak ref-counting, then m_counts
  // will be non-null.

  mutable T* m_master;
  mutable nub::ref_counts* m_counts;

}; // end nub::detail::weak_handle<T>

static const char vcid_groovx_nub_refdetail_h_utc20050630202900[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_REFDETAIL_H_UTC20050630202900DEFINED
