/***************************************************************************
 *
 * vector - declarations for the Standard Library vector class
 *
 **************************************************************************/

#ifndef MINIVEC_H_DEFINED
#define MINIVEC_H_DEFINED

#include "src/util/strings.h"
#include "src/util/error.h"

#include <stdcomp.h>

#include <new>

#include <limits>
#include <iterator>

namespace MVUtils {

  template <class T>
	 inline const T& max (const T& a, const T& b)
	 {
		return  a < b ? b : a;
	 }

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
		while (first != last) MVUtils::construct__(result++, *first++);
		return result;
	 }

  template <class ForwardIterator, class Size, class T>
	 inline void uninitialized_fill_n (ForwardIterator first, Size n, const T& x)
	 {
		while (n--) MVUtils::construct__(first++, x);
	 }

  // Default buffer size for containers.
  template <class T, class size>
	 inline size rw_allocation_size__(T*,size)
	 {
		return sizeof(T) >= 1024 ? 1 : 1024 / sizeof(T);
	 }


  template <class T>
	 inline void destroy__ (T* pointer)
	 {
		pointer->~T();
	 }


  template <class T1, class T2>
	 inline void construct__ (T1* p, const T2& value)
	 {
		new (p) T1(value);
	 }
}



//
// The default allocator.
//

class allocator {
public:

  typedef size_t    size_type;
  typedef ptrdiff_t difference_type;

  allocator() { ; }
  ~allocator() { ; }

  void * allocate (size_type n, void *  = 0)
    { 
      void * tmp =
		  static_cast<void*>(::operator new(static_cast<size_t>(n)));
		if (tmp == 0) throw bad_alloc();
		return tmp;
    }

  void deallocate (void* p) 
    { 
      ::operator delete(p);
    }

  size_type max_size (size_type size) const
    { 
      return 1 > UINT_MAX/size ? size_type(1) : size_type(UINT_MAX/size);
    }

};

//
// allocator_interface provides all types and typed functions.  Memory
// allocated as raw bytes using the class provided by the Allocator
// template parameter.  allocator_interface casts appropriately.
//
// Multiple allocator_interface objects can attach to a single 
// allocator, thus allowing one allocator to allocate all storage
// for a container, regardless of how many types are involved.
//
// The only real restriction is that pointer and reference are
// hard coded as T* and T&.  Partial specialization would 
// get around this.
//
template <class Allocator,class T>
class allocator_interface 
{
public:
  typedef Allocator allocator_type;
  typedef T*         pointer;
  typedef const T*   const_pointer;      
  typedef T&         reference;
  typedef const T&   const_reference;
  typedef T          value_type;
  typedef typename Allocator::size_type             size_type;
  typedef typename Allocator::difference_type       difference_type;
protected:
  allocator_type*         alloc_;

public:
  allocator_interface() : alloc_(NULL) { ; }
  allocator_interface(Allocator* a) : alloc_(a) { ; }

  void alloc(Allocator* a)
	 { 
		alloc_ = a; 
	 }   

  pointer address (T& x) 
	 { 
		return static_cast<pointer>(&x); 
	 }
  
  size_type max_size ()  const
	 { 
		return alloc_->max_size(sizeof(T));
	 }

  pointer allocate(size_type n, pointer = 0)
	 {
		return static_cast<pointer>(alloc_->allocate(n*sizeof(T)));
	 }

  void deallocate(pointer p)
	 {
		alloc_->deallocate(p);
	 }

  void construct(pointer p, const T& val)
	 {
		MVUtils::construct__(p,val);
	 }

  inline void destroy(T* p)
	 {
		MVUtils::destroy__(p);
	 }
};

template <>
class allocator_interface<allocator,void> 
{
public:
  typedef void*         pointer;
  typedef const void*   const_pointer;      
};


// 
// allocator globals
//
inline void * operator new(size_t N, allocator& a)
{
  return static_cast<void*>(a.allocate(N,(void*)0));
}


inline void * operator new[](size_t N, allocator& a)
{
  return static_cast<void*>(a.allocate(N));
}

inline bool operator==(const allocator&, const allocator&)
{
  return true;
}

class OutOfRange : public ErrorWithMsg {
public:
  OutOfRange (const char* msg) : ErrorWithMsg(msg) {;}
  virtual ~OutOfRange () { ; }
};

template <class T, class Allocator = allocator>
class minivec
{
public:
  //
  // Types.
  //
  typedef T                                          value_type;
  typedef Allocator                                  allocator_type;
  typedef typename Allocator::size_type              size_type;
  typedef typename Allocator::difference_type        difference_type;

  typedef typename allocator_interface<Allocator,T>::pointer          iterator;
  typedef typename allocator_interface<Allocator,T>::const_pointer    const_iterator;
  typedef typename allocator_interface<Allocator,T>::reference        reference;
  typedef typename allocator_interface<Allocator,T>::const_reference  const_reference;

protected:
  typedef typename allocator_interface<Allocator,T>::pointer          pointer;
  typedef typename allocator_interface<Allocator,T>::const_pointer    const_pointer;

public:
  typedef reverse_iterator<const_iterator, value_type, const_reference, 
	                        const_pointer, difference_type>
          const_reverse_iterator;
  typedef reverse_iterator<iterator, value_type,
	                        reference, pointer, difference_type>
          reverse_iterator;

protected:
  size_type buffer_size;
  allocator_type          the_allocator;

  allocator_interface<allocator_type,T> value_allocator;

  iterator start;
  iterator finish;
  iterator end_of_storage;

  void insert_aux (iterator position, const T& x);

  void destroy(iterator start_, iterator finish_)
    {
		while ( start_ != finish_)
		  value_allocator.destroy(start_++);
    }

public:
  //
  // construct/copy/destroy
  //
  explicit minivec (const Allocator& alloc = Allocator())
	 : start(0), finish(0), end_of_storage(0), the_allocator(alloc) 
    {
		initialize();
    }

  //
  // Build a minivec of size n with each element set to default for type T.
  // This requires that T have a default constructor.
  //
  explicit minivec (size_type n, const Allocator& alloc = Allocator())
	 : the_allocator(alloc)
    {
		initialize();
		start = value_allocator.allocate(n,(pointer)0);
		T value;
		MVUtils::uninitialized_fill_n(start, n, value);
		finish = start + n;
		end_of_storage = finish;
    }
	
  //
  // Build a minivec of size n with each element set to copy of value.
  //
  minivec (size_type n, const T& value, const Allocator& alloc = Allocator())
	 : the_allocator(alloc)
    {
		initialize();
		start = value_allocator.allocate(n,(pointer)0);
		MVUtils::uninitialized_fill_n(start, n, value);
		finish = start + n;
		end_of_storage = finish;
    }

  minivec (const minivec<T,Allocator>& x)
    {
		the_allocator = x.get_allocator();
		initialize();
		start = value_allocator.allocate(x.end() - x.begin(),(pointer)0);
		finish = MVUtils::uninitialized_copy(x.begin(), x.end(), start);
		end_of_storage = finish;
    }

  void initialize()
    {
		value_allocator.alloc(&the_allocator);
		buffer_size = 
		  MVUtils::max((size_type)1,MVUtils::rw_allocation_size__((value_type*)0,(size_type)0));
    }

  template<class InputIterator>
  minivec (InputIterator first, InputIterator last,
			  const Allocator& alloc = Allocator())
	 : the_allocator(alloc)
	 {
		size_type n;
		initialize();
		__initialize(n, size_type(0));
		distance(first, last, n);
		start = value_allocator.allocate(n,(pointer)0);
		finish = MVUtils::uninitialized_copy(first, last, start);
		end_of_storage = finish;
    }

  ~minivec ()
    { 
		destroy(start, finish); 
		value_allocator.deallocate(start);
    }

  minivec<T,Allocator>& operator= (const minivec<T,Allocator>& x);

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

  allocator_type get_allocator() const
    {
		return the_allocator;
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
  size_type max_size () const { return value_allocator.max_size();   }
  void resize (size_type new_size);
  void resize (size_type new_size, T value);

  size_type capacity () const { return size_type(end_of_storage - begin()); }
  bool      empty ()    const { return begin() == end();                    }
  void reserve (size_type n)
    {
		if (capacity() < n)
        {
			 iterator tmp = value_allocator.allocate(n,start);
			 MVUtils::uninitialized_copy(begin(), end(), tmp);
			 destroy(start, finish);
			 value_allocator.deallocate(start);
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
  
  reference       at (size_type n)               
    { 
		if (n >= size()) throw OutOfRange("index out of range");
      return *(begin() + n); 
    }

  const_reference at (size_type n)         const 
    { 
		if (n >= size()) throw OutOfRange("index out of range");
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
			 value_allocator.construct(finish, x); 
			 finish++;
        }
		else
		  insert_aux(end(), x);
    }
  void pop_back()
    {
		--finish; 
		value_allocator.destroy(finish);
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
			 value_allocator.construct(finish, x); finish++;
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
			 value_allocator.construct(finish, x); finish++;
        }
		else
		  insert_aux(position, x);
		return begin() + n;
    }

  void insert (iterator position, size_type n, const T& x);

  template<class InputIterator>
  void insert (iterator position, InputIterator first, InputIterator last);

  void swap (minivec<T,Allocator>& x)
    {
		if(the_allocator==x.the_allocator)
		  {
			 iterator x_start = x.start;
			 x.start = this->start;
			 this->start = x_start;

			 iterator x_finish = x.finish;
			 x.finish = this->finish;
			 this->finish = x_finish;

			 iterator x_end_of_storage = x.end_of_storage;
			 x.end_of_storage = this->end_of_storage;
			 this->end_of_storage = x_end_of_storage;
		  }
		else
		  {
			 minivec<T,Allocator> _x = *this;
			 *this = x;
			 x=_x;
		  } 
    }

  iterator erase (iterator position)
    {
		if (position + 1 != end()) copy(position + 1, end(), position);
		--finish;
		value_allocator.destroy(finish);
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

template <class T, class Allocator>
inline bool operator== (const minivec<T,Allocator>& x, const minivec<T,Allocator>& y)
{
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Allocator>
inline bool operator< (const minivec<T,Allocator>& x, const minivec<T,Allocator>& y)
{
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}


//
// This requires that T have a default constructor.
//

template <class T, class Allocator>
void minivec<T,Allocator>::resize (size_type new_size)
{
    T value;
    if (new_size > size())
        insert(end(), new_size - size(), value);
    else if (new_size < size())
        erase(begin() + new_size, end());
}

template <class T, class Allocator>
void minivec<T,Allocator>::resize (size_type new_size, T value)
{
    if (new_size > size())
        insert(end(), new_size - size(), value);
    else if (new_size < size())
        erase(begin() + new_size, end());
}

template <class T, class Allocator>
minivec<T,Allocator>& minivec<T,Allocator>::operator= (const minivec<T,Allocator>& x)
{
    if (&x == this) return *this;
    if (x.size() > capacity())
    {
        destroy(start, finish);
        value_allocator.deallocate(start);
        start = value_allocator.allocate(x.end() - x.begin(),(pointer)0);
        end_of_storage = MVUtils::uninitialized_copy(x.begin(), x.end(), start);
    }
    else if (size() >= x.size())
    {
        iterator i = copy(x.begin(), x.end(), begin());
        destroy(i, finish);
    }
    else
    {
        copy(x.begin(), x.begin() + size(), begin());
        MVUtils::uninitialized_copy(x.begin() + size(), x.end(), begin() + size());
    }
    finish = begin() + x.size();
    return *this;
}

template <class T, class Allocator>
void minivec<T,Allocator>::insert_aux (iterator position, const T& x)
{
    if (finish != end_of_storage)
    {
        value_allocator.construct(finish, *(finish - 1));
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
        size_type len = size() + buffer_size;
        iterator tmp = value_allocator.allocate(len,start);
        MVUtils::uninitialized_copy(begin(), position, tmp);
        value_allocator.construct((tmp + (position - begin())), x);
        MVUtils::uninitialized_copy(position, end(), tmp + (position - begin()) + 1); 
        destroy(begin(), end());
        value_allocator.deallocate(begin());
        end_of_storage = tmp + len;
        finish = tmp + size() + 1;
        start = tmp;
    }
}

template <class T, class Allocator>
void minivec<T,Allocator>::insert (iterator position, size_type n, const T& x)
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
        size_type len = size() + MVUtils::max(size(), n);
        iterator tmp = value_allocator.allocate(len,start);
        MVUtils::uninitialized_copy(begin(), position, tmp);
        MVUtils::uninitialized_fill_n(tmp + (position - begin()), n, x);
        MVUtils::uninitialized_copy(position, end(), tmp + (position - begin() + n));
        destroy(begin(), end());
        value_allocator.deallocate(begin());
        end_of_storage = tmp + len;
        finish = tmp + size() + n;
        start = tmp;
    }
}

template<class T, class Allocator>
template<class InputIterator>
void minivec<T,Allocator>::insert (iterator position, InputIterator first, 
											  InputIterator last)
{
    if (first == last) return;
    size_type n;
    __initialize(n, size_type(0));
    distance(first, last, n);
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
        iterator tmp = value_allocator.allocate(len,start);
        MVUtils::uninitialized_copy(begin(), position, tmp);
        MVUtils::uninitialized_copy(first, last, tmp + (position - begin()));
        MVUtils::uninitialized_copy(position, end(), tmp + (position - begin() + n));
        destroy(begin(), end());
        value_allocator.deallocate(begin());
        end_of_storage = tmp + len;
        finish = tmp + size() + n;
        start = tmp;
    }
}

#endif /*MINIVEC_H_DEFINED*/

