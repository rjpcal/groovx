///////////////////////////////////////////////////////////////////////
// tclobjlock.h
// Rob Peters
// created: Tue May 11 13:44:19 1999
// written: Tue Nov  9 13:28:05 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TCLOBJLOCK_H_DEFINED
#define TCLOBJLOCK_H_DEFINED

struct Tcl_Obj;

namespace {

  // This helper class can be used to ensure that a Tcl_Obj* is not
  // garbage-collected within a particular scope, by incrementing the
  // ref count on construction of the lock, and decrementing the ref
  // count on destruction of the lock. This allows Tcl_Obj*'s to
  // behave more according to normal scoping rules.
  class TclObjLock {
  public:
    TclObjLock(Tcl_Obj* obj) : itsObj(obj) { Tcl_IncrRefCount(itsObj); }
    ~TclObjLock() { Tcl_DecrRefCount(itsObj); }

	 TclObjLock(const TclObjLock& x)
	 {
		itsObj = const_cast<Tcl_Obj*>(x.itsObj);
		Tcl_IncrRefCount(itsObj);
	 }

	 TclObjLock& operator=(const TclObjLock& x)
	 {
		return this->operator=(x.itsObj);
	 }

	 TclObjLock& operator=(Tcl_Obj* x)
	 {
		if (itsObj != x) {
		  Tcl_DecrRefCount(itsObj);
		  itsObj = x;
		  Tcl_IncrRefCount(itsObj);
		}
		return *this;
	 }

  private:
    Tcl_Obj* itsObj;
  };

  class TclObjPtr {
  public:
	 TclObjPtr(Tcl_Obj* obj) : itsObj(obj) { Tcl_IncrRefCount(itsObj); }
	 ~TclObjPtr() { Tcl_DecrRefCount(itsObj); }
	 
	 TclObjPtr(const TclObjPtr& x)
	 {
		itsObj = const_cast<Tcl_Obj*>(x.itsObj);
		Tcl_IncrRefCount(itsObj);
	 }

	 TclObjPtr& operator=(const TclObjPtr& x)
	 {
		return this->operator=(x.itsObj);
	 }

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
	 operator Tcl_ObjPtr() { return itsObj; }
	 
  private:
	 Tcl_Obj* itsObj;
  };

}

static const char vcid_tclobjlock_h[] = "$Header$";
#endif // !TCLOBJLOCK_H_DEFINED
