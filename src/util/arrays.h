///////////////////////////////////////////////////////////////////////
//
// arrays.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Mar  6 15:56:36 2000
// written: Mon Mar  6 16:30:44 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ARRAYS_H_DEFINED
#define ARRAYS_H_DEFINED

#ifndef CSTDDEF_DEFINED
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

  T data[N];
};


static const char vcid_arrays_h[] = "$Header$";
#endif // !ARRAYS_H_DEFINED
