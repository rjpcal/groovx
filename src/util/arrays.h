///////////////////////////////////////////////////////////////////////
//
// arrays.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Mar  6 15:56:36 2000
// written: Wed Mar 22 16:47:00 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ARRAYS_H_DEFINED
#define ARRAYS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTDDEF)
#include <cstddef>
#define CSTDDEF_DEFINED
#endif

namespace Util
{
  class out_of_range {};
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c static_block is a simple wrapper around a C-style array that
 * provides an STL-style container interface, including both checked
 * and unchecked access, as well as access to the container's size
 * with the \c size() member function.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T, size_t N>
struct static_block {

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

  bool empty() const { return (N == 0); }

  void swap(static_block& other)
	 {
		static_block other_copy = other;
		other = *this;
		*this = other_copy;
	 }

  T data[N];
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * \c fixed_block is a simple wrapper for a dynamically allocated
 * array whose size is fixed on instantiation. Copying and assignment
 * are not allowed.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class fixed_block {
private:
  fixed_block(const fixed_block<T>& other);

  fixed_block<T>& operator=(const fixed_block<T>& other);

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

  bool empty() const { return (N == 0); }

  void swap(fixed_block& other)
	 {
		size_type other_N = other.N;
		other.N = this->N;
		this->N = other_N;

		T* other_data = other.data;
		other.data = this->data;
		this->data = other_data;
	 }

private:
  size_type N;
  T* data;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * \c dynamic_block is a simple wrapper for a dynamically allocated
 * array whose size may change with \c resize(). Copy construction and
 * copy assignment are allowed.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class dynamic_block {
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

  bool empty() const { return (N == 0); }

  void swap(dynamic_block& other)
	 {
		size_type other_N = other.N;
		other.N = this->N;
		this->N = other_N;

		T* other_data = other.data;
		other.data = this->data;
		this->data = other_data;
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
		  *ii++ = *start++;
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
