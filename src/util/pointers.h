///////////////////////////////////////////////////////////////////////
//
// pointers.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Mar  7 14:52:52 2000
// written: Tue Mar  7 15:07:38 2000
// $Id$
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
  explicit scoped_ptr( TT* p=0 ) throw() :
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

static const char vcid_pointers_h[] = "$Header$";
#endif // !POINTERS_H_DEFINED
