///////////////////////////////////////////////////////////////////////
//
// pointers.h
//
// Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Mar  7 14:52:52 2000
// written: Fri Nov 10 17:04:29 2000
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


///////////////////////////////////////////////////////////////////////
/**
 *
 *  \c scoped_ptr mimics a built-in pointer except that it guarantees
 *  deletion of the object pointed to, either on destruction of the
 *  scoped_ptr or via an explicit \c reset().
 *
 **/
///////////////////////////////////////////////////////////////////////

template<class T>
class scoped_ptr {
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
 *
 *  An enhanced relative of scoped_ptr with reference counted copy semantics.
 *  The object pointed to is deleted when the last shared_ptr pointing to it
 *  is destroyed or reset.
 *
 **/
///////////////////////////////////////////////////////////////////////

template<class T>
class shared_ptr {
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
		  catch (...) {
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
		T* other_px = other.px;
		other.px = this->px;
		this->px = other_px;
		long* other_pn = other.pn;
		other.pn = this->pn;
		this->pn = other_pn;
	 }

private:

  T*     px;     // contained pointer
  long*  pn;     // ptr to reference counter

  template<class TT> friend class shared_ptr;

  void dispose() { if (--*pn == 0) { delete px; delete pn; } }

  void share(T* rpx, long* rpn)
	 {
		if (pn != rpn) {
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

static const char vcid_pointers_h[] = "$Header$";
#endif // !POINTERS_H_DEFINED
