///////////////////////////////////////////////////////////////////////
//
// tclitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 12:33:59 1999
// written: Wed Jun 23 14:28:32 1999
// $Id$
//
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_H_DEFINED
#define TCLITEMPKG_H_DEFINED

#ifndef TCLPKG_H_DEFINED
#include "tclpkg.h"
#endif

#ifndef TCLCMD_H_DEFINED
#include "tclcmd.h"
#endif

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

template <class T> class Getter;
template <class T> class Setter;
template <class T> class Attrib;

class Action;

///////////////////////////////////////////////////////////////////////
//
// TclItemPkg class definition
//
///////////////////////////////////////////////////////////////////////

class TclItemPkg : public TclPkg {
public:
  TclItemPkg(Tcl_Interp* interp, const char* name, const char* version,
				 int item_argn=1) :
	 TclPkg(interp, name, version), 
	 itsItemArgn(item_argn) {}

  virtual void* getItemFromId(int id) = 0;

  int itemArgn() const { return itsItemArgn; }
  
protected:
  template <class T>
  void declareGetter(const char* cmd_name, Getter<T>* getter)
  { declareGetter_(this, cmd_name, getter); }
  
  template <class T>
  void declareSetter(const char* cmd_name, Setter<T>* setter)
  { declareSetter_(this, cmd_name, setter); }

  template <class T>
  void declareAttrib(const char* attrib_name, Attrib<T>* attrib)
  { declareAttrib_(this, attrib_name, attrib); }

  void declareAction(const char* action_name, Action* action);

private:
  template <class T>
  static void declareGetter_(TclItemPkg*, const char*, Getter<T>*);

  template <class T>
  static void declareSetter_(TclItemPkg*, const char*, Setter<T>*);

  template <class T>
  static void declareAttrib_(TclItemPkg*, const char*, Attrib<T>*);

  void instantiate();

  int itsItemArgn;
};

///////////////////////////////////////////////////////////////////////
//
// TclIoItemPkg class definition
//
// This subclass of TclItemPkg automatically creates appropriate
// "stringify" and "destringify" commands in the Tcl package. To do
// this, it requires overriding a function to retrieve an IO reference.
//
///////////////////////////////////////////////////////////////////////

class IO;

class TclIoItemPkg : public TclItemPkg {
public:
  TclIoItemPkg(Tcl_Interp* interp, const char* name, const char* version,
					int item_argn=1);

  virtual IO& getIoFromId(int id) = 0;
  virtual int getBufSize() = 0;
};

///////////////////////////////////////////////////////////////////////
//
// CTclIoItemPkg class definition
//
///////////////////////////////////////////////////////////////////////

template <class C>
class CTclIoItemPkg : public TclIoItemPkg {
public:
  CTclIoItemPkg(Tcl_Interp* interp, const char* name, const char* version,
					 int item_argn=1) :
	 TclIoItemPkg(interp, name, version, item_argn) {}

  virtual C* getCItemFromId(int id) = 0;
  virtual void* getItemFromId(int id) {
	 return static_cast<void*>(getCItemFromId(id));
  }
};

///////////////////////////////////////////////////////////////////////
//
// TclItemCmd class definition
//
///////////////////////////////////////////////////////////////////////

template <class T>
class TclItemCmd : public TclCmd {
public:
  TclItemCmd(TclItemPkg* pkg, const char* cmd_name, const char* usage, 
				 int objc_min=0, int objc_max=100000, bool exact_objc=false) :
	 TclCmd(pkg->interp(), cmd_name, usage, objc_min, objc_max, exact_objc),
	 itsPkg(pkg) {}
protected:
  virtual void invoke() = 0;

  T* getItem() {
	 int id = itsPkg->itemArgn() ? getIntFromArg(itsPkg->itemArgn()) : -1;
	 return static_cast<T*>(itsPkg->getItemFromId(id));
  }

private:
  TclItemPkg* itsPkg;
};

///////////////////////////////////////////////////////////////////////
//
// Attrib template class definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
class Getter {
public:
  virtual T get(void* item) const = 0;
};

template <class T>
class Setter {
public:
  virtual void set(void* item, T val) = 0;
};

template <class C, class T>
class CGetter : public virtual Getter<T> {
public:
  typedef T (C::* Getter_f) () const;

  CGetter(Getter_f getter) : itsGetter_f(getter) {}

  virtual T get(void *item) const {
	 const C* p = static_cast<C*>(item);
	 return (p->*itsGetter_f)();
  }

private:
  Getter_f itsGetter_f;
};

template <class C, class T>
class CSetter : public virtual Setter<T> {
public:
  typedef void (C::* Setter_f) (T);

  CSetter(Setter_f setter) : itsSetter_f(setter) {}

  virtual void set(void* item, T val) {
	 C* p = static_cast<C*>(item);
	 (p->*itsSetter_f)(val);
  }

private:
  Setter_f itsSetter_f;
};

template <class T>
class Attrib : public virtual Getter<T>, public virtual Setter<T> {};

template <class C, class T>
class CAttrib : public Attrib<T>, public CGetter<C,T>, public CSetter<C,T> {
public:
  typedef T (C::* Getter_f) () const;
  typedef void (C::* Setter_f) (T);

  CAttrib(Getter_f getter, Setter_f setter) :
	 CGetter<C,T>(getter),
	 CSetter<C,T>(setter) {}
};

class Action {
public:
  virtual void action(void* item) = 0;
};

template <class C>
class CAction: public Action {
public:
  typedef void (C::* Action_f) ();

  CAction(Action_f action_f) : itsAction_f(action_f) {}

  virtual void action(void* item) {
	 C* p = static_cast<C*>(item);
	 (p->*itsAction_f)();
  }

private:
  Action_f itsAction_f;
};

template <class C>
class CConstAction : public Action {
public:
  typedef void (C::* Action_f) () const;

  CConstAction(Action_f action_f) : itsAction_f(action_f) {}

  virtual void action(void* item) {
	 const C* p = static_cast<C*>(item);
	 (p->*itsAction_f)();
  }

private:
  Action_f itsAction_f;
};

static const char vcid_tclitempkg_h[] = "$Header$";
#endif // !TCLITEMPKG_H_DEFINED
