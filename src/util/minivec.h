///////////////////////////////////////////////////////////////////////
//
// minivec.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Oct 31 11:01:16 2000
// written: Wed Sep 25 18:58:58 2002
// $Id$
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

namespace MVUtils
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
      while (first != last) MVUtils::construct(result++, *first++);
      return result;
    }

  template <class ForwardIterator, class Size, class T>
    inline void uninitialized_fill_n (ForwardIterator first, Size n, const T& x)
    {
      while (n--) MVUtils::construct(first++, x);
    }

  template <class T>
    inline void destroy (T* pointer)
    {
      pointer->~T();
    }


  template <class T1, class T2>
    inline void construct (T1* p, const T2& value)
    {
      new (p) T1(value);
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

  void destroy(iterator start_, iterator finish_)
    {
      while ( start_ != finish_)
        MVUtils::destroy(start_++);
    }

  static size_type mv_max_size()
    {
      return 1 > UINT_MAX/sizeof(T) ?
        size_type(1) : size_type(UINT_MAX/sizeof(T));
    }

  static pointer mv_allocate(size_type n, pointer = 0)
    {
      void * tmp =
        static_cast<void*>(::operator new(static_cast<size_t>(n*sizeof(T))));
      if (tmp == 0) throw std::bad_alloc();
      return static_cast<pointer>(tmp);
    }

  static void mv_deallocate(pointer p)
    {
      ::operator delete(static_cast<void*>(p));
    }

public:
  //
  // construct/copy/destroy
  //
  explicit minivec () :
    start(0), finish(0), end_of_storage(0)
    {}

  //
  // Build a minivec of size n with each element set to default for type T.
  // This requires that T have a default constructor.
  //
  explicit minivec (size_type n)
    {
      start = mv_allocate(n,(pointer)0);
      T value;
      MVUtils::uninitialized_fill_n(start, n, value);
      finish = start + n;
      end_of_storage = finish;
    }

  //
  // Build a minivec of size n with each element set to copy of value.
  //
  minivec (size_type n, const T& value)
    {
      start = mv_allocate(n,(pointer)0);
      MVUtils::uninitialized_fill_n(start, n, value);
      finish = start + n;
      end_of_storage = finish;
    }

  minivec (const minivec<T>& x)
    {
      start = mv_allocate(x.end() - x.begin(),(pointer)0);
      finish = MVUtils::uninitialized_copy(x.begin(), x.end(), start);
      end_of_storage = finish;
    }

  template<class InputIterator>
  minivec (InputIterator first, InputIterator last)
    {
      size_type n = std::distance(first, last);
      start = mv_allocate(n,(pointer)0);
      finish = MVUtils::uninitialized_copy(first, last, start);
      end_of_storage = finish;
    }

  ~minivec ()
    {
      destroy(start, finish);
      mv_deallocate(start);
    }

  minivec<T>& operator= (const minivec<T>& x);

  template<class InputIterator>
  void assign (InputIterator first, InputIterator last)
    {
      erase(begin(), end()); insert(begin(), first, last);
    }

  //
  // Assign n copies of default value of type T to minivec.
  // This requires that T have a default constructor.
  //
  template<class Size>
  void assign (Size n)
    {
      erase(begin(), end()); insert(begin(), n, T());
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
  iterator       begin ()       { return start;  }
  const_iterator begin () const { return start;  }
  iterator       end ()         { return finish; }
  const_iterator end ()   const { return finish; }

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
  size_type max_size () const { return mv_max_size();   }
  void resize (size_type new_size);
  void resize (size_type new_size, T value);

  size_type capacity () const { return size_type(end_of_storage - begin()); }
  bool      empty ()    const { return begin() == end();                    }
  void reserve (size_type n)
    {
      if (capacity() < n)
        {
          iterator tmp = mv_allocate(n,start);
          MVUtils::uninitialized_copy(begin(), end(), tmp);
          destroy(start, finish);
          mv_deallocate(start);
          finish = tmp + size();
          start = tmp;
          end_of_storage = begin() + n;
        }
    }

  //
  // Element access.
  //
  reference       operator[] (size_type n)
    {
      return *(begin() + n);
    }

  const_reference operator[] (size_type n) const
    {
      return *(begin() + n);
    }

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
          MVUtils::construct(finish, x);
          finish++;
        }
      else
        insert_aux(end(), x);
    }

  void pop_back()
    {
      --finish;
      MVUtils::destroy(finish);
    }

  //
  // Insert default value of type T at position.
  // Requires that T have a default constructor.
  //
  iterator insert (iterator position)
    {
      size_type n = position - begin();
      T x;
      if (finish != end_of_storage && position == end())
        {
          MVUtils::construct(finish, x); finish++;
        }
      else
        insert_aux(position, x);
      return begin() + n;
    }

  //
  // Insert x at position.
  //
  iterator insert (iterator position, const T& x)
    {
      size_type n = position - begin();
      if (finish != end_of_storage && position == end())
        {
          MVUtils::construct(finish, x); finish++;
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
      if (position + 1 != end()) MVUtils::copy(position + 1, end(), position);
      --finish;
      MVUtils::destroy(finish);
      return position;
    }

  iterator erase (iterator first, iterator last)
    {
      iterator i = MVUtils::copy(last, end(), first);
      destroy(i, finish);
      finish = finish - (last - first);
      return first;
    }
  void clear()
    {
      erase(begin(),end());
    }

}; // end class minivec

//
// This requires that T have a default constructor.
//

template <class T>
void minivec<T>::resize (size_type new_size)
{
  T value;
  if (new_size > size())
    insert(end(), new_size - size(), value);
  else if (new_size < size())
    erase(begin() + new_size, end());
}

template <class T>
void minivec<T>::resize (size_type new_size, T value)
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
      destroy(start, finish);
      mv_deallocate(start);
      start = mv_allocate(x.end() - x.begin(),(pointer)0);
      end_of_storage = MVUtils::uninitialized_copy(x.begin(), x.end(), start);
    }
  else if (size() >= x.size())
    {
      iterator i = MVUtils::copy(x.begin(), x.end(), begin());
      destroy(i, finish);
    }
  else
    {
      MVUtils::copy(x.begin(), x.begin() + size(), begin());
      MVUtils::uninitialized_copy(x.begin() + size(), x.end(), begin() + size());
    }
  finish = begin() + x.size();
  return *this;
}

template <class T>
void minivec<T>::insert_aux (iterator position, const T& x)
{
  if (finish != end_of_storage)
    {
      MVUtils::construct(finish, *(finish - 1));
      MVUtils::copy_backward(position, finish - 1, finish);
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
      iterator tmp = mv_allocate(len,start);
      MVUtils::uninitialized_copy(begin(), position, tmp);
      MVUtils::construct((tmp + (position - begin())), x);
      MVUtils::uninitialized_copy(position, end(), tmp + (position - begin()) + 1);
      destroy(begin(), end());
      mv_deallocate(begin());
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
          MVUtils::uninitialized_copy(end() - n, end(), end());
          MVUtils::copy_backward(position, end() - n, end());
          MVUtils::fill(position, position + n, x);
        }
      else
        {
          MVUtils::uninitialized_copy(position, end(), position + n);
          MVUtils::fill(position, end(), x);
          MVUtils::uninitialized_fill_n(end(), n - (end() - position), x);
        }
      finish += n;
    }
  else
    {
      size_type len = size() + max(size(), n);
      iterator tmp = mv_allocate(len,start);
      MVUtils::uninitialized_copy(begin(), position, tmp);
      MVUtils::uninitialized_fill_n(tmp + (position - begin()), n, x);
      MVUtils::uninitialized_copy(position, end(), tmp + (position - begin() + n));
      destroy(begin(), end());
      mv_deallocate(begin());
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
          MVUtils::uninitialized_copy(end() - n, end(), end());
          MVUtils::copy_backward(position, end() - n, end());
          copy(first, last, position);
        }
      else
        {
          MVUtils::uninitialized_copy(position, end(), position + n);
          copy(first, first + (end() - position), position);
          MVUtils::uninitialized_copy(first + (end() - position), last, end());
        }
      finish += n;
    }
  else
    {
      size_type len = size() + max(size(), n);
      iterator tmp = mv_allocate(len,start);
      MVUtils::uninitialized_copy(begin(), position, tmp);
      MVUtils::uninitialized_copy(first, last, tmp + (position - begin()));
      MVUtils::uninitialized_copy(position, end(), tmp + (position - begin() + n));
      destroy(begin(), end());
      mv_deallocate(begin());
      end_of_storage = tmp + len;
      finish = tmp + size() + n;
      start = tmp;
    }
}

static const char vcid_minivec_h[] = "$Header$";
#endif // !MINIVEC_H_DEFINED
