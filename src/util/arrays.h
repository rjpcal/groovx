///////////////////////////////////////////////////////////////////////
//
// arrays.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Mar  6 15:56:36 2000
// written: Wed Mar 19 13:34:05 2003
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

/// Get a pointer to the beginning of a C-style array.
template<class T, std::size_t N>
T* array_begin(T (&array)[N])
{
  return &array[0];
}

/// Get a pointer to the one-past-the-end of a C-style array.
template<class T, std::size_t N>
T* array_end(T (&array)[N])
{
  return &array[0]+N;
}

/// Get a pointer to the beginning of a C-style const array.
template<class T, std::size_t N>
const T* array_begin(T const (&array)[N])
{
  return &array[0];
}

/// Get a pointer to the one-past-the-end of a C-style const array.
template<class T, std::size_t N>
const T* array_end(T const (&array)[N])
{
  return &array[0]+N;
}


//  ###################################################################
//  ===================================================================

/// A simple wrapper around a C-style array.
/** \c static_block provides an STL-style container interface, including
    both checked and unchecked access, as well as access to the container's
    size with the \c size() member function. */

template <class T, size_t N>
struct static_block
{
  typedef T value_type;                     ///< STL value type.

  typedef T* pointer;                       ///< STL pointer type.
  typedef const T* const_pointer;           ///< STL const pointer type.
  typedef T& reference ;                    ///< STL reference type.
  typedef const T& const_reference;         ///< STL const reference type.

  typedef pointer iterator;                 ///< STL iterator type.
  typedef const_pointer const_iterator;     ///< STL const iterator type.

  typedef size_t size_type;                 ///< STL size type.
  typedef ptrdiff_t difference_type;        ///< STL iterator difference type.

  iterator begin() { return data; }         ///< Iterator to array start.
  iterator end() { return data+N; }         ///< Iterator to array one-past-the-end.

  const_iterator begin() const { return data; } ///< Const iterator to array start.
  const_iterator end() const { return data+N; } ///< Const iterator to array one-past-the-end.

  /// Unchecked non-const array index.
  reference operator[](size_type n) { return data[n]; }

  /// Unchecked const array index.
  const_reference operator[](size_type n) const { return data[n]; }

  /// Checked non-const array index (Util::out_of_range thrown on bounds error).
  reference at(size_type n)
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  /// Checked const array index (Util::out_of_range thrown on bounds error).
  const_reference at(size_type n) const
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  /// Size of array.
  size_type size() const { return N; }

  /// Maximum size of array type.
  size_type max_size() const { return size_type(-1) / sizeof(T); }

  /// Query whether size is zero.
  bool is_empty() const { return (N == 0); }

  /// Swap data with another array.
  /** This requires an element-by-element swap. */
  void swap(static_block& other)
    {
      Util::swap2(*this, other);
    }

  T data[N];
};


//  ###################################################################
//  ===================================================================

/// A simple wrapper for a dynamic array whose size is fixed at construction.
/** Copying and assignment are not  allowed. */

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
  typedef T value_type;                     ///< STL value type.

  typedef T* pointer;                       ///< STL pointer type.
  typedef const T* const_pointer;           ///< STL const pointer type.
  typedef T& reference ;                    ///< STL reference type.
  typedef const T& const_reference;         ///< STL const reference type.

  typedef pointer iterator;                 ///< STL iterator type.
  typedef const_pointer const_iterator;     ///< STL const iterator type.

  typedef size_t size_type;                 ///< STL size type.
  typedef ptrdiff_t difference_type;        ///< STL iterator difference type.

  /// Construct with a given size.
  fixed_block(size_type n) : N(n), data(new T[N]) {}

  /// Destructor.
  ~fixed_block() { delete [] data; }

  /// Construct by copying from a given iterator range.
  template <class Itr>
  fixed_block(Itr itr, Itr end) : N(end-itr), data(new T[N])
    {
      assign(itr, end);
    }

  iterator begin() { return data; }         ///< Iterator to array start.
  iterator end() { return data+N; }         ///< Iterator to array one-past-the-end.

  const_iterator begin() const { return data; } ///< Const iterator to array start.
  const_iterator end() const { return data+N; } ///< Const iterator to array one-past-the-end.

  /// Unchecked non-const array index.
  reference operator[](size_type n) { return data[n]; }

  /// Unchecked const array index.
  const_reference operator[](size_type n) const { return data[n]; }

  /// Checked non-const array index (Util::out_of_range thrown on bounds error).
  reference at(size_type n)
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  /// Checked const array index (Util::out_of_range thrown on bounds error).
  const_reference at(size_type n) const
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  /// Size of array.
  size_type size() const { return N; }

  /// Maximum size of array type.
  size_type max_size() const { return size_type(-1) / sizeof(T); }

  /// Query whether size is zero.
  bool is_empty() const { return (N == 0); }

  /// Swap with another fixed_block.
  /** This is fast since it only requires swapping the interal pointers to
      the dynamically-allocated arrays; no element-wise swap is needed. */
  void swap(fixed_block& other)
    {
      Util::swap2(N,    other.N);
      Util::swap2(data, other.data);
    }

private:
  size_type N;
  T* data;
};


//  ###################################################################
//  ===================================================================

/// A reference-counted smart pointer for arrays.

/** The array pointed to is deleted when the last shared_array pointing to
    it is destroyed or reset. */

template<typename T>
class shared_array
{
public:
  /// The pointed-to type.
  typedef T element_type;

  /// Construct with the given array pointer.
  /** Ownership is now unconditionally transferred to the shared_array. If
      the shared_array constructor causes an exception, the pointed-to
      array will be destroyed. */
  explicit shared_array(T* p =0) : px(p)
  {
    try { pn = new long(1); }  // fix: prevent leak if new throws
    catch (...) { delete [] p; throw; }
  }

  /// Copy constructor.
  shared_array(const shared_array& r) : px(r.px) throw()
  { ++*(pn = r.pn); }

  /// Destructor.
  ~shared_array() { dispose(); }

  /// Assignment oeprator.
  shared_array& operator=(const shared_array& r)
  {
    if (pn != r.pn)
      { // Q: why not px != r.px? A: fails when both px == 0
        dispose();
        px = r.px;
        ++*(pn = r.pn);
      }
    return *this;
  }

  /// Reset to point to a new array.
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

  /// Get a pointer to the pointed-to array.
  T* get() const                     throw() { return px; }

  /// Index into the pointed-to array.
  T& operator[](std::size_t i) const throw() { return px[i]; }

  /// Get the reference count of the shared array.
  long use_count() const             throw() { return *pn; }

  /// Query whether the shared array is uniquely owned (i.e. refcount == 1).
  bool unique() const                throw() { return *pn == 1; }

  /// Swap pointees with another shared_array.
  void swap(shared_array<T>& other) throw()
  { Util::swap2(px,other.px); Util::swap2(pn,other.pn); }

private:

  T*     px;     // contained pointer
  long*  pn;     // ptr to reference counter

  void dispose() { if (--*pn == 0) { delete [] px; delete pn; } }

};  // shared_array

/// Equality for shared_array objects; returns true if both have the same pointee.
template<typename T>
  inline bool operator==(const shared_array<T>& a, const shared_array<T>& b)
    { return a.get() == b.get(); }

/// Inequality for shared_array objects; returns true if each has different pointees.
template<typename T>
  inline bool operator!=(const shared_array<T>& a, const shared_array<T>& b)
    { return a.get() != b.get(); }



//  ###################################################################
//  ===================================================================

/// A wrapper for a dynamically allocated array whose size may be changed.

template <class T>
class dynamic_block
{
public:
  typedef T value_type;                     ///< STL value type.

  typedef T* pointer;                       ///< STL pointer type.
  typedef const T* const_pointer;           ///< STL const pointer type.
  typedef T& reference ;                    ///< STL reference type.
  typedef const T& const_reference;         ///< STL const reference type.

  typedef pointer iterator;                 ///< STL iterator type.
  typedef const_pointer const_iterator;     ///< STL const iterator type.

  typedef size_t size_type;                 ///< STL size type.
  typedef ptrdiff_t difference_type;        ///< STL iterator difference type.

  /// Construct with a given size.
  dynamic_block(size_type n = 0) : N(n), data(new T[N]) {}

  /// Copy construct.
  dynamic_block(const dynamic_block<T>& other) :
    N(other.N), data(new T[N])
    {
      assign_varsize(other, *this);
    }

  /// Assignment operator.
  dynamic_block<T>& operator=(const dynamic_block<T>& other)
    {
      dynamic_block temp(other);
      this->swap(temp);
      return *this;
    }

  /// Destructor.
  ~dynamic_block() { delete [] data; }

  iterator begin() { return data; }         ///< Iterator to array start.
  iterator end() { return data+N; }         ///< Iterator to array one-past-the-end.

  const_iterator begin() const { return data; } ///< Const iterator to array start.
  const_iterator end() const { return data+N; } ///< Const iterator to array one-past-the-end.

  /// Unchecked non-const array index.
  reference operator[](size_type n) { return data[n]; }

  /// Unchecked const array index.
  const_reference operator[](size_type n) const { return data[n]; }

  /// Checked non-const array index (Util::out_of_range thrown on bounds error).
  reference at(size_type n)
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  /// Checked const array index (Util::out_of_range thrown on bounds error).
  const_reference at(size_type n) const
    {
      if ( n >= N )
        throw Util::out_of_range();
      return data[n];
    }

  /// Size of array.
  size_type size() const { return N; }

  /// Maximum size of array type.
  size_type max_size() const { return size_type(-1) / sizeof(T); }

  /// Query whether size is zero.
  bool is_empty() const { return (N == 0); }

  /// Swap with another dynamic_block.
  /** This is fast since it only requires swapping the interal pointers to
      the dynamically-allocated arrays; no element-wise swap is needed. */
  void swap(dynamic_block& other)
    {
      Util::swap2(N,    other.N);
      Util::swap2(data, other.data);
    }

  /// Resize to a new size.
  void resize(size_type new_size)
    {
      dynamic_block temp(new_size);
      assign_varsize(*this, temp);
      this->swap(temp);
    }

  /// Assign from a given iterator range.
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
  /// Assign one dynamic_block to another whose sizes may differ.
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
