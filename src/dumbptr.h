///////////////////////////////////////////////////////////////////////
//
// dumbptr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb  1 17:58:49 2000
// written: Tue Feb  1 18:14:46 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DUMBPTR_H_DEFINED
#define DUMBPTR_H_DEFINED

template <class T>
class DumbPtr {
private:
  T* itsPtr;

public:
  template <class TT>
  explicit DumbPtr(TT* ptr = 0) :
	 itsPtr(ptr)
	 {}

  ~DumbPtr() {}

  DumbPtr(const DumbPtr<T>& other) :
	 itsPtr(other.itsPtr)
	 {}

  template <class TT>
  DumbPtr(const DumbPtr<TT>& other) :
	 itsPtr(other.itsPtr)
	 {}

  DumbPtr<T>& operator=(const DumbPtr<T>& other)
	 { itsPtr = other.itsPtr; return *this; }
 
  template <class TT>
  DumbPtr<T>& operator=(const DumbPtr<TT>& other)
	 { itsPtr = other.itsPtr; return *this; }
 
        T* operator->()       { return itsPtr; }
  const T* operator->() const { return itsPtr; }
        T& operator*()        { return *itsPtr; }
  const T& operator*()  const { return *itsPtr; }

        T* get()              { return itsPtr; }
  const T* get()        const { return itsPtr; }

  bool operator==(const DumbPtr<T>& rhs) const
	 { return itsPtr == rhs.itsPtr; }

  bool operator!=(const DumbPtr<T>& rhs) const
    { return itsPtr != rhs.itsPtr; }

  bool operator==(T* rhs) const
    { return itsPtr == rhs; }

  bool operator!=(T* rhs) const
    { return itsPtr != rhs; }

  bool operator!() const
    { return itsPtr == 0; }
};

static const char vcid_dumbptr_h[] = "$Header$";
#endif // !DUMBPTR_H_DEFINED
