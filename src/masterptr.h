///////////////////////////////////////////////////////////////////////
//
// masterptr.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct  9 08:18:28 2000
// written: Mon Oct 16 14:34:19 2000
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

  /// Default constructor.
  MasterPtrBase();

  /// Increment the object's reference count.
  void incrRefCount();

  /** Decrement the object's reference count. If this causes the
      reference count to fall to zero or below, the pointee and the
      pointer will be destroyed by a call to 'delete this'. */
  void decrRefCount();

  /// Returns true if the reference count is greater than one.
  bool isShared() const;

  /// Returns true if the reference count is one or less.
  bool isUnshared() const;

  /// Returns the object's reference count.
  int refCount() const;

  /** Returns true if there is a valid pointee (the logical negation
      of isNull()). */
  bool isValid() const;

  /// Returns true if the pointee is null.
  virtual bool isNull() const = 0;

  /** Returns true if the pointee of \a other is logically the same as
      the pointee of \a this. */
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

  /// Overridden from \c MasterPtrBase to always return true.
  virtual bool isNull() const;

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

  virtual bool isNull() const = 0;

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

  virtual bool isNull() const;

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



///////////////////////////////////////////////////////////////////////
/**
 *
 * NullablePtrHandle<T> is a stack-based wrapper for ref-counted
 * MasterPtr<T>'s, similar to PtrHandle<T>, except that a
 * NullablePtrHandle may point to no MasterPtr<T> at all. isValid()
 * may be called to determine if there is currently a non-null pointee.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class NullablePtrHandle {
public:
  explicit NullablePtrHandle(MasterPtr<T>* master) : itsMaster(master)
    {
		if (itsMaster != 0)
		  itsMaster->incrRefCount();
	 }

  explicit NullablePtrHandle(PtrHandle<T> other) :
	 itsMaster(other.masterPtr())
    {
		ensureValid();
		itsMaster->incrRefCount();
	 }

  ~NullablePtrHandle()
    {
		if (itsMaster != 0)
		  itsMaster->decrRefCount();
	 }

  NullablePtrHandle(const NullablePtrHandle& other) : itsMaster(other.itsMaster)
    {
		if (itsMaster != 0)
		  itsMaster->incrRefCount();
	 }

  NullablePtrHandle& operator=(const NullablePtrHandle& other)
    {
		NullablePtrHandle otherCopy(other);
		this->swap(otherCopy);
		return *this;
	 }

  NullablePtrHandle& operator=(const PtrHandle<T>& other)
    {
		NullablePtrHandle otherCopy(other);
		this->swap(otherCopy);
		return *this;
	 }

  bool isValid() const { return itsMaster != 0; }

  void release()
    {
		if (itsMaster != 0)
		  itsMaster->decrRefCount();
		itsMaster = 0;
	 }

        MasterPtr<T>* masterPtr()       { return itsMaster; }
  const MasterPtr<T>* masterPtr() const { return itsMaster; }

        T* operator->()       { return get(); }
  const T* operator->() const { return get(); }
        T& operator*()        { return *(get()); }
  const T& operator*()  const { return *(get()); }

  T* get()
  {
	 ensureValid();
	 return (itsMaster->getPtr());
  }

  const T* get() const
  {
	 ensureValid();
	 return (itsMaster->getPtr());
  }

private:
  void ensureValid()
  {
	 if (itsMaster == 0)
		MasterPtrBase::throwErrorWithMsg(
			  "attempted to derefence invalid NullablePtrHandle");
  }

  void swap(NullablePtrHandle& other)
    {
		MasterPtr<T>* otherMaster = other.itsMaster;
		other.itsMaster = this->itsMaster;
		this->itsMaster = otherMaster;
	 }

  MasterPtr<T>* itsMaster;
};

static const char vcid_masterptr_h[] = "$Header$";
#endif // !MASTERPTR_H_DEFINED
