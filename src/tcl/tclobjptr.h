///////////////////////////////////////////////////////////////////////
// tclobjlock.h
// Rob Peters
// created: Tue May 11 13:44:19 1999
// written: Tue Dec  7 18:13:00 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TCLOBJLOCK_H_DEFINED
#define TCLOBJLOCK_H_DEFINED

struct Tcl_Obj;

namespace Tcl {

///////////////////////////////////////////////////////////////////////
/**
 *
 * This utility class manages the reference count of a Tcl_Obj within
 * a lexical scope. It increments the ref count on construction of the
 * lock, and decrements the ref count on destruction of the lock. This
 * allows Tcl_Obj*'s to behave according to normal scoping rules.
 *
 **/
///////////////////////////////////////////////////////////////////////

class TclObjLock {
public:
  /// Default constructor.
  TclObjLock(Tcl_Obj* obj) : itsObj(obj) { Tcl_IncrRefCount(itsObj); }

  /// Destructor.
  ~TclObjLock() { Tcl_DecrRefCount(itsObj); }
  
  /// Copy constructor.
  TclObjLock(const TclObjLock& x)
	 {
		itsObj = const_cast<Tcl_Obj*>(x.itsObj);
		Tcl_IncrRefCount(itsObj);
	 }
  
  /// Assignment operator from a TclObjLock.
  TclObjLock& operator=(const TclObjLock& x)
	 {
		return this->operator=(x.itsObj);
	 }

  /// Assignment operator from a Tcl_Obj*.
  TclObjLock& operator=(Tcl_Obj* x)
	 {
		if (itsObj != x) {		  Tcl_DecrRefCount(itsObj);
		  itsObj = x;
		  Tcl_IncrRefCount(itsObj);
		}
		return *this;
	 }

private:
  Tcl_Obj* itsObj;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * This class acts as a reference counted smart pointer to Tcl_Obj. It
 * is similar to TclObjLock in that it manages the reference count of
 * a held Tcl_Obj*, but it also defines a conversion operator back to
 * Tcl_Obj* in order to provide access to Tcl_Obj's interface.
 *
 **/
///////////////////////////////////////////////////////////////////////

class TclObjPtr {
public:
  /// Default constructor.
  TclObjPtr(Tcl_Obj* obj) : itsObj(obj) { Tcl_IncrRefCount(itsObj); }

  /// Destructor.
  ~TclObjPtr() { Tcl_DecrRefCount(itsObj); }
  
  /// Copy constructor.
  TclObjPtr(const TclObjPtr& x)
	 {
		itsObj = const_cast<Tcl_Obj*>(x.itsObj);
		Tcl_IncrRefCount(itsObj);
	 }
  
  /// Assignment operator from TclObjPtr.
  TclObjPtr& operator=(const TclObjPtr& x)
	 {
		return this->operator=(x.itsObj);
	 }
  
  /// Assignment operator from Tcl_Obj*.
  TclObjPtr& operator=(Tcl_Obj* x)
	 {
		if (itsObj != x) {
		  Tcl_DecrRefCount(itsObj);
		  itsObj = x;
		  Tcl_IncrRefCount(itsObj);
		}
		return *this;
	 }
  
  typedef Tcl_Obj* Tcl_ObjPtr;

  /// Conversion operator to Tcl_Obj*.
  operator Tcl_ObjPtr() { return itsObj; }
  
private:
  Tcl_Obj* itsObj;
};

} // end namespace Tcl

static const char vcid_tclobjlock_h[] = "$Header$";
#endif // !TCLOBJLOCK_H_DEFINED
