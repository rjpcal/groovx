///////////////////////////////////////////////////////////////////////
//
// masterptr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct  9 08:18:28 2000
// written: Mon Oct  9 13:01:11 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASTERPTR_H_DEFINED
#define MASTERPTR_H_DEFINED

///////////////////////////////////////////////////////////////////////
/**
 *
 * MasterPtrBase is a reference counting base class for master
 * pointers.
 *
 **/
///////////////////////////////////////////////////////////////////////

class MasterPtrBase {
private:
  int itsRefCount;

  // These are disallowed since MasterPtrBase's should always be in
  // one-to-one correspondence with their pointee's.
  MasterPtrBase(const MasterPtrBase& other);
  MasterPtrBase& operator=(const MasterPtrBase& other);

protected:
  /** Virtual destructor is protected, so that we can prevent clients
		from instantiating MasterPtr's on the stack and from destroying
		them explicitly. Instead, MasterPtr's will only be destroyed by
		a 'delete this' call inside decrRefCount() if the reference
		count falls to zero or below. Clients are forced to create
		MasterPtr's dynamically using \c new, which is what we need in
		order for 'delete this' to be valid later on. */
  virtual ~MasterPtrBase();

public:
  /// Utility function to throw an \c ErrorWithMsg exception.
  static void throwErrorWithMsg(const char* msg);

  MasterPtrBase();

  void incrRefCount();
  void decrRefCount();

  int refCount() const;

  virtual bool isValid() const = 0;

  virtual bool operator==(const MasterPtrBase& other) = 0;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * NullMasterPtr
 *
 **/
///////////////////////////////////////////////////////////////////////

class NullMasterPtr : public MasterPtrBase {
protected:
  /// Virtual destructor.
  virtual ~NullMasterPtr();

public:
  /// Default constructor.
  NullMasterPtr();

  /// Overridden from \c MasterPtrBase to always returns false.
  virtual bool isValid() const;

  /// Returns true if and only if \a other is also a \c NullMasterPtr.
  virtual bool operator==(const MasterPtrBase& other);
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * MasterIoPtr
 *
 **/
///////////////////////////////////////////////////////////////////////

namespace IO { class IoObject; }

class MasterIoPtr : public MasterPtrBase {
protected:
  virtual ~MasterIoPtr();

public:
  MasterIoPtr();

  virtual IO::IoObject* ioPtr() const = 0;

  virtual bool isValid() const = 0;

  virtual bool operator==(const MasterPtrBase& other) = 0;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * MasterPtr<T> template
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class MasterPtr : public MasterIoPtr {
private:
  T* itsPtr;

protected:
  virtual ~MasterPtr();

public:
  explicit MasterPtr(T* ptr);

        T* getPtr()       { return itsPtr; }
  const T* getPtr() const { return itsPtr; }

  virtual IO::IoObject* ioPtr() const;

  virtual bool isValid() const;

  virtual bool operator==(const MasterPtrBase& other);
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * PtrHandle<T> is a stack-based wrapper for ref-counted MasterPtr<T>'s.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class PtrHandle {
public:
  explicit PtrHandle(MasterPtr<T>* master) : itsMaster(master)
  {
	 if (master == 0) MasterPtrBase::throwErrorWithMsg(
								"MasterPtr<T>* was null in PtrHandle<T>()");
	 itsMaster->incrRefCount();
  }

  ~PtrHandle()
    { itsMaster->decrRefCount(); }

  PtrHandle(const PtrHandle& other) : itsMaster(other.itsMaster)
    { itsMaster->incrRefCount(); }

  PtrHandle& operator=(const PtrHandle& other)
    {
		PtrHandle otherCopy(other);
		this->swap(otherCopy);
		return *this;
	 }

        MasterPtr<T>* masterPtr()       { return itsMaster; }
  const MasterPtr<T>* masterPtr() const { return itsMaster; }

        T* operator->()       { return (itsMaster->getPtr()); }
  const T* operator->() const { return (itsMaster->getPtr()); }
        T& operator*()        { return *(itsMaster->getPtr()); }
  const T& operator*()  const { return *(itsMaster->getPtr()); }

        T* get()              { return (itsMaster->getPtr()); }
  const T* get()        const { return (itsMaster->getPtr()); }

private:
  void swap(PtrHandle& other)
    {
		MasterPtr<T>* otherMaster = other.itsMaster;
		other.itsMaster = this->itsMaster;
		this->itsMaster = otherMaster;
	 }

  MasterPtr<T>* itsMaster;
};

static const char vcid_masterptr_h[] = "$Header$";
#endif // !MASTERPTR_H_DEFINED
