///////////////////////////////////////////////////////////////////////
//
// arrays.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  6 15:56:36 2000
// written: Mon Jan 13 11:08:26 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ARRAYS_H_DEFINED
#define ARRAYS_H_DEFINED

#include "util/algo.h"

#include <cstddef>

namespace Util
{
  /// Exception class for range errors.
  /** Used in static_block, fixed_block, dynamic_block. */
  class out_of_range {};
}


template<class T, std::size_t N>
T* array_begin(T (&array)[N])
{
  return &array[0];
}

template<class T, std::size_t N>
T* array_end(T (&array)[N])
{
  return &array[0]+N;
}

template<class T, std::size_t N>
const T* array_begin(T const (&array)[N])
{
  return &array[0];
}

template<class T, std::size_t N>
const T* array_end(T const (&array)[N])
{
  return &array[0]+N;
}

///////////////////////////////////////////////////////////////////////
/**
 * \c static_block is a simple wrapper around a C-style array that
 * provides an STL-style container interface, including both checked
 * and unchecked access, as well as access to the container's size
 * with the \c size() member function.
 **/
///////////////////////////////////////////////////////////////////////

template <class T, size_t N>
struct static_block
{

  typedef T value_type;

  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  typedef pointer iterator;
  typedef const_pointer const_iterator;

  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  iterator begin() { return data; }
  iterator end() { return data+N; }

  const_iterator begin() const { return data; }
  const_iterator end() const { return data+N; }

  reference operator[](size_type n) { return data[n]; }
  const_reference operator[](size_type n) const { return data[n]; }

  reference at(size_type n)
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  const_reference at(size_type n) const
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  size_type size() const { return N; }

  size_type max_size() const { return size_type(-1) / sizeof(T); }

  bool is_empty() const { return (N == 0); }

  void swap(static_block& other)
    {
      Util::swap2(*this, other);
    }

  T data[N];
};


///////////////////////////////////////////////////////////////////////
/**
 * \c fixed_block is a simple wrapper for a dynamically allocated array
 * whose size is fixed at construction. Copying and assignment are not
 * allowed.
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class fixed_block
{
private:
  fixed_block(const fixed_block<T>& other);

  fixed_block<T>& operator=(const fixed_block<T>& other);

  template <class Itr>
  void assign(Itr thatitr, Itr thatend)
    {
      iterator thisitr = begin(), thisend = end();

      while (thatitr != thatend && thisitr != thisend)
        {
          *thisitr = *thatitr;
          ++thisitr;
          ++thatitr;
        }
    }

public:

  typedef T value_type;

  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  typedef pointer iterator;
  typedef const_pointer const_iterator;

  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  fixed_block(size_type n) : N(n), data(new T[N]) {}
  ~fixed_block() { delete [] data; }

  template <class Itr>
  fixed_block(Itr itr, Itr end) : N(end-itr), data(new T[N])
    {
      assign(itr, end);
    }

  iterator begin() { return data; }
  iterator end() { return data+N; }

  const_iterator begin() const { return data; }
  const_iterator end() const { return data+N; }

  reference operator[](size_type n) { return data[n]; }
  const_reference operator[](size_type n) const { return data[n]; }

  reference at(size_type n)
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  const_reference at(size_type n) const
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  size_type size() const { return N; }

  size_type max_size() const { return size_type(-1) / sizeof(T); }

  bool is_empty() const { return (N == 0); }

  void swap(fixed_block& other)
    {
      Util::swap2(N,    other.N);
      Util::swap2(data, other.data);
    }

private:
  size_type N;
  T* data;
};


///////////////////////////////////////////////////////////////////////
/**
 *  shared_array extends shared_ptr to arrays. The array pointed to is
 *  deleted when the last shared_array pointing to it is destroyed or
 *  reset.
 **/
///////////////////////////////////////////////////////////////////////

template<typename T>
class shared_array
{
public:
  typedef T element_type;

  explicit shared_array(T* p =0) : px(p)
  {
    try { pn = new long(1); }  // fix: prevent leak if new throws
    catch (...) { delete [] p; throw; }
  }

  shared_array(const shared_array& r) : px(r.px)  // never throws
  { ++*(pn = r.pn); }

  ~shared_array() { dispose(); }

  shared_array& operator=(const shared_array& r)
  {
    if (pn != r.pn)
      { // Q: why not px != r.px? A: fails when both px == 0
        dispose();
        px = r.px;
        ++*(pn = r.pn);
      }
    return *this;
  } // operator=

  void reset(T* p=0)
  {
    if ( px == p ) return;  // fix: self-assignment safe
    if (--*pn == 0) { delete [] px; }
    else { // allocate new reference counter
      try { pn = new long; }  // fix: prevent leak if new throws
      catch (...)
      {
        ++*pn;  // undo effect of --*pn above to meet effects guarantee
        delete [] p;
        throw;
      } // catch
    } // allocate new reference counter
    *pn = 1;
    px = p;
  } // reset

  T* get() const                     { return px; }  // never throws
  T& operator[](std::size_t i) const { return px[i]; }  // never throws

  long use_count() const             { return *pn; }  // never throws
  bool unique() const                { return *pn == 1; }  // never throws

  void swap(shared_array<T>& other)  // never throws
  { Util::swap2(px,other.px); Util::swap2(pn,other.pn); }

private:

  T*     px;     // contained pointer
  long*  pn;     // ptr to reference counter

  void dispose() { if (--*pn == 0) { delete [] px; delete pn; } }

};  // shared_array

template<typename T>
  inline bool operator==(const shared_array<T>& a, const shared_array<T>& b)
    { return a.get() == b.get(); }

template<typename T>
  inline bool operator!=(const shared_array<T>& a, const shared_array<T>& b)
    { return a.get() != b.get(); }


///////////////////////////////////////////////////////////////////////
/**
 * \c dynamic_block is a simple wrapper for a dynamically allocated
 * array whose size may change with \c resize(). Copy construction and
 * copy assignment are allowed.
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class dynamic_block
{
public:

  typedef T value_type;

  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  typedef pointer iterator;
  typedef const_pointer const_iterator;

  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  dynamic_block(size_type n = 0) : N(n), data(new T[N]) {}

  dynamic_block(const dynamic_block<T>& other) :
    N(other.N), data(new T[N])
    {
      assign_varsize(other, *this);
    }

  dynamic_block<T>& operator=(const dynamic_block<T>& other)
    {
      dynamic_block temp(other);
      this->swap(temp);
      return *this;
    }

  ~dynamic_block() { delete [] data; }

  iterator begin() { return data; }
  iterator end() { return data+N; }

  const_iterator begin() const { return data; }
  const_iterator end() const { return data+N; }

  reference operator[](size_type n) { return data[n]; }
  const_reference operator[](size_type n) const { return data[n]; }

  reference at(size_type n)
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  const_reference at(size_type n) const
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  size_type size() const { return N; }

  size_type max_size() const { return size_type(-1) / sizeof(T); }

  bool is_empty() const { return (N == 0); }

  void swap(dynamic_block& other)
    {
      Util::swap2(N,    other.N);
      Util::swap2(data, other.data);
    }

  void resize(size_type new_size)
    {
      dynamic_block temp(new_size);
      assign_varsize(*this, temp);
      this->swap(temp);
    }

  template <class RandomAccessIterator>
  void assign(RandomAccessIterator start, RandomAccessIterator finish)
    {
      int num = finish-start;
      if (num < 0)
        {
          resize(0);
          return;
        }
      resize(num);
      iterator ii = begin();
      while (start != finish)
        {
          *ii = *start;
          ++ii;
          ++start;
        }
    }

private:
  static void assign_varsize(const dynamic_block& b_from, dynamic_block& b_to)
    {
      const_iterator from = b_from.begin();
      iterator to = b_to.begin();

      while(from != b_from.end() && to != b_to.end())
        {
          *to++ = *from++;
        }
    }

  size_type N;
  T* data;
};


static const char vcid_arrays_h[] = "$Header$";
#endif // !ARRAYS_H_DEFINED
