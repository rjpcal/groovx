///////////////////////////////////////////////////////////////////////
//
// minivec.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Oct 31 11:01:16 2000
// written: Wed Mar 19 17:58:54 2003
// $Id$
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

#ifndef MINIVEC_H_DEFINED
#define MINIVEC_H_DEFINED

#include "util/algo.h"

#include <iterator>
#include <new>

#ifdef HAVE_LIMITS
#include <limits>
#endif

namespace mvaux
{

  template <class BidirectionalIterator1, class BidirectionalIterator2>
    inline BidirectionalIterator2 copy_backward (BidirectionalIterator1 first,
                                                 BidirectionalIterator1 last,
                                                 BidirectionalIterator2 result)
    {
      while (first != last) *--result = *--last;
      return result;
    }

  template <class ForwardIterator, class T>
    inline void fill (ForwardIterator first, ForwardIterator last, const T& value)
    {
      while (first != last) *first++ = value;
    }

  template <class InputIterator, class OutputIterator>
    inline OutputIterator copy (InputIterator first, InputIterator last,
                                OutputIterator result)
    {
      while (first != last) *result++ = *first++;
      return result;
    }

  //
  // Specialized algorithms.
  //

  template <class InputIterator, class ForwardIterator>
    inline ForwardIterator uninitialized_copy (InputIterator first,
                                               InputIterator last,
                                               ForwardIterator result)
    {
      while (first != last) mvaux::construct(result++, *first++);
      return result;
    }

  template <class ForwardIterator, class Size, class T>
    inline ForwardIterator uninitialized_fill_n (ForwardIterator dest,
                                                 Size n, const T& x)
    {
      while (n--) mvaux::construct(dest++, x);
      return dest;
    }

  template <class T>
    inline void destroy (T* pointer)
    {
      pointer->~T();
    }

  template <class T>
    inline void destroy(T* start, T* finish)
    {
      while ( start < finish)
        mvaux::destroy(start++);
    }


  template <class T1, class T2>
    inline void construct (T1* p, const T2& value)
    {
      new (p) T1(value);
    }

  template <class T>
  inline size_t max_size()
    {
      return 1 > UINT_MAX/sizeof(T) ?
        size_t(1) : size_t(UINT_MAX/sizeof(T));
    }

  template <class T>
  inline T* allocate(size_t n)
    {
      void* tmp =
        static_cast<void*>(::operator new(static_cast<size_t>(n*sizeof(T))));
      if (tmp == 0) throw std::bad_alloc();
      return static_cast<T*>(tmp);
    }

  template <class T>
  inline void deallocate(T* p)
    {
      ::operator delete(static_cast<void*>(p));
    }
}


/// Resizable 1-D array class based on slimmed-down version of std::vector<>.
template <class T>
class minivec
{
public:
  //
  // Types.
  //
  typedef T           value_type;
  typedef size_t      size_type;
  typedef ptrdiff_t   difference_type;

  typedef T*          pointer;
  typedef const T*    const_pointer;
  typedef T&          reference;
  typedef const T&    const_reference;

  typedef T*          iterator;
  typedef const T*    const_iterator;

  typedef std::reverse_iterator<iterator>       reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

private:
  iterator start;
  iterator finish;
  iterator end_of_storage;

  void insert_aux (iterator position, const T& x);

public:
  //
  // construct/copy/destroy
  //
  explicit minivec () :
    start(0), finish(0), end_of_storage(0)
    {}

  //
  // Build a minivec of size n with each element set to copy of value.
  //
  minivec (size_type n, const T& value = T())
    {
      start = mvaux::allocate<T>(n);
      finish = mvaux::uninitialized_fill_n(start, n, value);
      end_of_storage = finish;
    }

  minivec (const minivec<T>& x)
    {
      start = mvaux::allocate<T>(x.end() - x.begin());
      finish = mvaux::uninitialized_copy(x.begin(), x.end(), start);
      end_of_storage = finish;
    }

  template<class InputIterator>
  minivec (InputIterator first, InputIterator last)
    {
      size_type n = std::distance(first, last);
      start = mvaux::allocate<T>(n);
      finish = mvaux::uninitialized_copy(first, last, start);
      end_of_storage = finish;
    }

  ~minivec ()
    {
      mvaux::destroy(start, finish);
      mvaux::deallocate(start);
    }

  minivec<T>& operator= (const minivec<T>& x);

  template<class InputIterator>
  void assign (InputIterator first, InputIterator last)
    {
      erase(begin(), end()); insert(begin(), first, last);
    }

  //
  // Assign n copies of t to this minivec.
  //
  template<class Size, class X>
  void assign (Size n, const X& t)
    {
      erase(begin(), end()); insert(begin(), n, t);
    }

  //
  // Iterators.
  //
  iterator       begin()       { return start;  }
  const_iterator begin() const { return start;  }
  iterator       end()         { return finish; }
  const_iterator end()   const { return finish; }

  reverse_iterator rbegin ()
    {
      reverse_iterator tmp(end()); return tmp;
    }
  const_reverse_iterator rbegin () const
    {
      const_reverse_iterator tmp(end()); return tmp;
    }
  reverse_iterator rend ()
    {
      reverse_iterator tmp(begin()); return tmp;
    }
  const_reverse_iterator rend () const
    {
      const_reverse_iterator tmp(begin()); return tmp;
    }

  //
  // Capacity.
  //
  size_type size ()     const { return size_type(end() - begin()); }
  size_type max_size () const { return mvaux::max_size<T>();   }

  void resize (size_type new_size, const T& value = T());

  size_type capacity () const { return size_type(end_of_storage - begin()); }
  bool      empty ()    const { return begin() == end();                    }
  void reserve (size_type n)
    {
      if (capacity() < n)
        {
          pointer tmp = mvaux::allocate<T>(n);
          mvaux::uninitialized_copy(begin(), end(), tmp);
          mvaux::destroy(start, finish);
          mvaux::deallocate(start);
          finish = tmp + size();
          start = tmp;
          end_of_storage = begin() + n;
        }
    }

  //
  // Element access.
  //
  reference       operator[] (size_type n)       { return *(begin() + n); }
  const_reference operator[] (size_type n) const { return *(begin() + n); }

  /// Exeception class of index-out-of-range errors in minivec.
  class out_of_range {};

  reference       at (size_type n)
    {
      if (n >= size()) throw out_of_range();
      return *(begin() + n);
    }

  const_reference at (size_type n)         const
    {
      if (n >= size()) throw out_of_range();
      return *(begin() + n);
    }

  reference       front ()                       { return *begin();       }
  const_reference front ()                 const { return *begin();       }
  reference       back ()                        { return *(end() - 1);   }
  const_reference back ()                  const { return *(end() - 1);   }

  //
  // Modifiers.
  //
  void push_back (const T& x)
    {
      if (finish != end_of_storage)
        {
          mvaux::construct(finish, x);
          ++finish;
        }
      else
        insert_aux(end(), x);
    }

  void pop_back()
    {
      --finish;
      mvaux::destroy(finish);
    }

  //
  // Insert x at position.
  //
  iterator insert (iterator position, const T& x = T())
    {
      size_type n = position - begin();
      if (finish != end_of_storage && position == end())
        {
          mvaux::construct(finish, x); finish++;
        }
      else
        insert_aux(position, x);
      return begin() + n;
    }

  void insert (iterator position, size_type n, const T& x);

  template<class InputIterator>
  void insert (iterator position, InputIterator first, InputIterator last);

  void swap (minivec<T>& x)
    {
      Util::swap2(start, x.start);
      Util::swap2(finish, x.finish);
      Util::swap2(end_of_storage, x.end_of_storage);
    }

  iterator erase (iterator position)
    {
      if (position + 1 != end()) mvaux::copy(position + 1, end(), position);
      --finish;
      mvaux::destroy(finish);
      return position;
    }

  iterator erase (iterator first, iterator last)
    {
      iterator i = mvaux::copy(last, end(), first);
      mvaux::destroy(i, finish);
      finish = finish - (last - first);
      return first;
    }

  void clear()
    {
      erase(begin(),end());
    }

}; // end class minivec

template <class T>
void minivec<T>::resize (size_type new_size, const T& value)
{
  if (new_size > size())
    insert(end(), new_size - size(), value);
  else if (new_size < size())
    erase(begin() + new_size, end());
}

template <class T>
minivec<T>& minivec<T>::operator= (const minivec<T>& x)
{
  if (&x == this) return *this;
  if (x.size() > capacity())
    {
      mvaux::destroy(start, finish);
      mvaux::deallocate(start);
      start = mvaux::allocate<T>(x.end() - x.begin());
      end_of_storage = mvaux::uninitialized_copy(x.begin(), x.end(), start);
    }
  else if (size() >= x.size())
    {
      iterator i = mvaux::copy(x.begin(), x.end(), begin());
      mvaux::destroy(i, finish);
    }
  else
    {
      mvaux::copy(x.begin(), x.begin() + size(), begin());
      mvaux::uninitialized_copy(x.begin() + size(), x.end(), begin() + size());
    }
  finish = begin() + x.size();
  return *this;
}

template <class T>
void minivec<T>::insert_aux (iterator position, const T& x)
{
  if (finish != end_of_storage)
    {
      mvaux::construct(finish, *(finish - 1));
      mvaux::copy_backward(position, finish - 1, finish);
      *position = x;
      ++finish;
    }
  else
    {
      //
      // We always allocate enough space for a number of additional
      // elements in the minivec, unless the size of each element is
      // very large.
      //
      const size_type buffer_size = sizeof(T) >= 1024 ? 1 : 1024/sizeof(T);
      size_type len = size() + buffer_size;
      pointer tmp = mvaux::allocate<T>(len);
      iterator middle = mvaux::uninitialized_copy(begin(), position, tmp);
      mvaux::construct(middle, x);
      mvaux::uninitialized_copy(position, end(), middle + 1);
      mvaux::destroy(begin(), end());
      mvaux::deallocate(begin());
      end_of_storage = tmp + len;
      finish = tmp + size() + 1;
      start = tmp;
    }
}

template <class T>
void minivec<T>::insert (iterator position, size_type n, const T& x)
{
  if (n == 0) return;
  if (end_of_storage - finish >= n)
    {
      if (end() - position > n)
        {
          mvaux::uninitialized_copy(end() - n, end(), end());
          mvaux::copy_backward(position, end() - n, end());
          mvaux::fill(position, position + n, x);
        }
      else
        {
          mvaux::uninitialized_copy(position, end(), position + n);
          mvaux::fill(position, end(), x);
          mvaux::uninitialized_fill_n(end(), n - (end() - position), x);
        }
      finish += n;
    }
  else
    {
      size_type len = size() + max(size(), n);
      pointer tmp = mvaux::allocate<T>(len);
      iterator middle = mvaux::uninitialized_copy(begin(), position, tmp);
      mvaux::uninitialized_fill_n(middle, n, x);
      mvaux::uninitialized_copy(position, end(), middle + n);
      mvaux::destroy(begin(), end());
      mvaux::deallocate(begin());
      end_of_storage = tmp + len;
      finish = tmp + size() + n;
      start = tmp;
    }
}

template<class T>
template<class InputIterator>
void minivec<T>::insert (iterator position, InputIterator first,
                         InputIterator last)
{
  if (first == last) return;
  size_type n = std::distance(first, last);
  if (end_of_storage - finish >= n)
    {
      if (end() - position > n)
        {
          mvaux::uninitialized_copy(end() - n, end(), end());
          mvaux::copy_backward(position, end() - n, end());
          mvaux::copy(first, last, position);
        }
      else
        {
          mvaux::uninitialized_copy(position, end(), position + n);
          mvaux::copy(first, first + (end() - position), position);
          mvaux::uninitialized_copy(first + (end() - position), last, end());
        }
      finish += n;
    }
  else
    {
      size_type len = size() + max(size(), n);
      pointer tmp = mvaux::allocate<T>(len);
      iterator middle = mvaux::uninitialized_copy(begin(), position, tmp);
      mvaux::uninitialized_copy(first, last, middle);
      mvaux::uninitialized_copy(position, end(), middle + n);
      mvaux::destroy(begin(), end());
      mvaux::deallocate(begin());
      end_of_storage = tmp + len;
      finish = tmp + size() + n;
      start = tmp;
    }
}

static const char vcid_minivec_h[] = "$Header$";
#endif // !MINIVEC_H_DEFINED
