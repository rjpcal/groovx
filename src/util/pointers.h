///////////////////////////////////////////////////////////////////////
//
// pointers.h
//
// Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Mar  7 14:52:52 2000
// commit: $Id$
//
// -------------------------------------------------------------------
//
// Borrowed and modified from boost.org smart_ptr.hpp
// Original copyright notice:
//
//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999. Permission
//  to copy, use, modify, sell and distribute this software is granted
//  provided this copyright notice appears in all copies. This
//  software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.
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

#ifndef POINTERS_H_DEFINED
#define POINTERS_H_DEFINED

#include "util/algo.h"

//  ###################################################################
//  ===================================================================

/// A smart-pointer for unshared objects.

/** \c scoped_ptr mimics a built-in pointer except that it guarantees
    deletion of the object pointed to, either on destruction of the
    scoped_ptr or via an explicit \c reset(). */

template<class T>
class scoped_ptr
{
public:
  /// Construct with pointer to given object (or null).
  explicit scoped_ptr( T* p=0 ) throw() :
    ptr(p)
    {}

  /// Construct with pointer to given object of related type.
  template <class TT>
  explicit scoped_ptr( TT* p ) throw() :
    ptr(p)
    {}

  /// Destructor.
  ~scoped_ptr()
    { delete ptr; }

  /// Reset with pointer to different object (or null).
  void reset( T* p=0 )
    {
      if ( ptr != p )
        { delete ptr; ptr = p; }
    }

  /// Dereference.
  T& operator*() const throw()
    { return *ptr; }

  /// Dereference for member access.
  T* operator->() const throw()
    { return ptr; }

  /// Get a pointer to the referred-to object.
  T* get() const throw()
    { return ptr; }

private:
  scoped_ptr(const scoped_ptr& other);
  scoped_ptr& operator=(const scoped_ptr& other);

  T* ptr;
};


//  ###################################################################
//  ===================================================================

/// A reference-counted smart pointer for shared objects.

/** \c shared_ptr is an enhanced relative of scoped_ptr with reference
    counted copy semantics.  The object pointed to is deleted when the last
    shared_ptr pointing to it is destroyed or reset. */

template<class T>
class shared_ptr
{
public:
  typedef T element_type;

  /// Construct with pointer to given object (or null).
  explicit shared_ptr(T* p =0) :
    px(p), pn(0)
    {
      try { pn = new long(1); }  // fix: prevent leak if new throws
      catch (...) { delete p; throw; }
    }

  /// Copy construct.
  shared_ptr(const shared_ptr& r) throw() :
    px(r.px), pn(r.pn)
    {
      ++(*pn);
    }

  /// Destructor.
  ~shared_ptr() { dispose(); }

  /// Assignment operator.
  shared_ptr& operator=(const shared_ptr& r)
    {
      share(r.px,r.pn);
      return *this;
    }

  /// Copy constructor from pointer to related type.
  template<class TT>
  shared_ptr(const shared_ptr<TT>& r) throw() :
    px(r.px), pn(r.pn)
    {
      ++(*pn);
    }

  /// Assignment operator from pointer to related type.
  template<class TT>
  shared_ptr& operator=(const shared_ptr<TT>& r)
    {
      share(r.px,r.pn);
      return *this;
    }

  /// Reset with a pointer to a different object (or null).
  void reset(T* p=0)
    {
      if ( px == p ) return;  // fix: self-assignment safe
      if (--*pn == 0) { delete px; }
      else { // allocate new reference counter
        try { pn = new long; }  // fix: prevent leak if new throws
        catch (...)
        {
          ++*pn;  // undo effect of --*pn above to meet effects guarantee
          delete p;
          throw;
        } // catch
      } // allocate new reference counter
      *pn = 1;
      px = p;
    }

  /// Dereference.
  T& operator*() const throw()
    { return *px; }

  /// Dereference for member access.
  T* operator->() const throw()
    { return px; }

  /// Get a pointer to the referred-to object.
  T* get() const throw()
    { return px; }

  /// Get the current reference count.
  long use_count() const throw()
    { return *pn; }

  /// Query whether the pointee is unique (i.e. refcount == 1).
  bool unique() const throw()
    { return *pn == 1; }

  /// Swap pointees with another shared_ptr.
  void swap(shared_ptr<T>& other) throw()
    {
      Util::swap2(px, other.px);
      Util::swap2(pn, other.pn);
    }

private:

  T*     px;     // contained pointer
  long*  pn;     // ptr to reference counter

  template<class TT> friend class shared_ptr;

  void dispose() { if (--*pn == 0) { delete px; delete pn; } }

  void share(T* rpx, long* rpn)
    {
      if (pn != rpn)
        {
          dispose();
          px = rpx;
          ++*(pn = rpn);
        }
    }
};

/// Equality for shared_ptr; returns true if both have the same pointee.
template<class T, class U>
inline bool operator==(const shared_ptr<T>& a, const shared_ptr<U>& b)
{
  return a.get() == b.get();
}

/// Inequality for shared_ptr; returns true if each has different pointees.
template<class T, class U>
inline bool operator!=(const shared_ptr<T>& a, const shared_ptr<U>& b)
{
  return a.get() != b.get();
}

/// A factory helper function to make a shared_ptr from a raw pointer.
template <class T>
shared_ptr<T> make_shared(T* t) { return shared_ptr<T>(t); }

static const char vcid_pointers_h[] = "$Header$";
#endif // !POINTERS_H_DEFINED
