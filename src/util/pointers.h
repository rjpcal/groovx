///////////////////////////////////////////////////////////////////////
//
// pointers.h
//
// Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Mar  7 14:52:52 2000
// written: Wed Sep 25 18:59:44 2002
// $Id$
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
///////////////////////////////////////////////////////////////////////

#ifndef POINTERS_H_DEFINED
#define POINTERS_H_DEFINED

#include "util/algo.h"

/// A borrowed pointer class
/** The sole purpose of this class is to explicitly state the fact that a
    given pointer is "borrowed", and does not for example need to be
    deep-copied when its owner is copied, and hence to avoid compiler
    warnings to that effect. */
template <class T>
class borrowed_ptr
{
private:
  T* ptr;

public:
  borrowed_ptr( T* p=0 ) throw() :
    ptr(p)
  {}

  ~borrowed_ptr() throw() {}

  template <class TT>
  borrowed_ptr( TT* p ) throw() :
    ptr(p)
  {}

  borrowed_ptr( const borrowed_ptr& other ) throw() :
    ptr(other.ptr)
  {}

  template <class TT>
  borrowed_ptr( const borrowed_ptr<TT>& other ) throw() :
    ptr(other.ptr)
  {}

  borrowed_ptr& operator=( T* p ) throw()
  { ptr = p; return *this; }

  borrowed_ptr& operator=( const borrowed_ptr& other ) throw()
  { ptr = other.ptr; return *this; }

  template <class TT>
  borrowed_ptr& operator=( const borrowed_ptr<TT>& other ) throw()
  { ptr = other.ptr; return *this; }

  bool operator==( T* p ) const throw()
  { return ptr == p; }

  bool operator==( const borrowed_ptr& other ) const throw()
  { return ptr == other.ptr; }

  template <class TT>
  bool operator==( const borrowed_ptr<TT>& other ) const throw()
  { return ptr == other.ptr; }

  typedef T* pointer;
  typedef T& reference;

  reference operator*() const throw() { return *ptr; }

  pointer operator->() const throw() { return ptr; }

  operator pointer() const throw() { return ptr; }
};

///////////////////////////////////////////////////////////////////////
/**
 *  \c scoped_ptr mimics a built-in pointer except that it guarantees
 *  deletion of the object pointed to, either on destruction of the
 *  scoped_ptr or via an explicit \c reset().
 **/
///////////////////////////////////////////////////////////////////////

template<class T>
class scoped_ptr
{
public:
  explicit scoped_ptr( T* p=0 ) throw() :
    ptr(p)
    {}

  template <class TT>
  explicit scoped_ptr( TT* p ) throw() :
    ptr(p)
    {}

  ~scoped_ptr()
    { delete ptr; }

  void reset( T* p=0 )
    {
      if ( ptr != p )
        { delete ptr; ptr = p; }
    }

  T& operator*() const throw()
    { return *ptr; }

  T* operator->() const throw()
    { return ptr; }

  T* get() const throw()
    { return ptr; }

private:
  scoped_ptr(const scoped_ptr& other);
  scoped_ptr& operator=(const scoped_ptr& other);

  T* ptr;
};

///////////////////////////////////////////////////////////////////////
/**
 *  An enhanced relative of scoped_ptr with reference counted copy semantics.
 *  The object pointed to is deleted when the last shared_ptr pointing to it
 *  is destroyed or reset.
 **/
///////////////////////////////////////////////////////////////////////

template<class T>
class shared_ptr
{
public:
  typedef T element_type;

  explicit shared_ptr(T* p =0) :
    px(p), pn(0)
    {
      try { pn = new long(1); }  // fix: prevent leak if new throws
      catch (...) { delete p; throw; }
    }

  shared_ptr(const shared_ptr& r) throw() :
    px(r.px), pn(r.pn)
    {
      ++(*pn);
    }

  ~shared_ptr() { dispose(); }

  shared_ptr& operator=(const shared_ptr& r)
    {
      share(r.px,r.pn);
      return *this;
    }

  template<class TT>
  shared_ptr(const shared_ptr<TT>& r) throw() :
    px(r.px), pn(r.pn)
    {
      ++(*pn);
    }

  template<class TT>
  shared_ptr& operator=(const shared_ptr<TT>& r)
    {
      share(r.px,r.pn);
      return *this;
    }

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

  T& operator*() const throw()
    { return *px; }

  T* operator->() const throw()
    { return px; }

  T* get() const throw()
    { return px; }

  long use_count() const throw()
    { return *pn; }

  bool unique() const throw()
    { return *pn == 1; }

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

template<class T, class U>
inline bool operator==(const shared_ptr<T>& a, const shared_ptr<U>& b)
{
  return a.get() == b.get();
}

template<class T, class U>
inline bool operator!=(const shared_ptr<T>& a, const shared_ptr<U>& b)
{
  return a.get() != b.get();
}

template <class T>
shared_ptr<T> make_shared(T* t) { return shared_ptr<T>(t); }

static const char vcid_pointers_h[] = "$Header$";
#endif // !POINTERS_H_DEFINED
