///////////////////////////////////////////////////////////////////////
//
// tclitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 12:33:59 1999
// written: Thu Mar 30 08:29:17 2000
// $Id$
//
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_H_DEFINED
#define TCLITEMPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLITEMPKGBASE_H_DEFINED)
#include "tcl/tclitempkgbase.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCMD_H_DEFINED)
#include "tcl/tclcmd.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PROPERTY_H_DEFINED)
#include "io/property.h"
#endif

namespace IO { class IoObject; }

///////////////////////////////////////////////////////////////////////
//
// Attrib template class definitions
//
///////////////////////////////////////////////////////////////////////

/**
 *
 * \c CGetter implements the \c Getter interface by calling a member
 * function of the template class \c C.
 *
 **/
template <class C, class T>
class CGetter : public virtual Getter<T> {
public:
  /// The type of member function pointer that will be called in \c get().
  typedef T (C::* Getter_f) () const;

  /// Construct with a member function pointer.
  CGetter(Getter_f getter) : itsGetter_f(getter) {}

  /// Casts \a item to type \c C* and calls the stored member function.
  virtual T get(void *item) const {
	 const C* p = static_cast<C*>(item);
	 return (p->*itsGetter_f)();
  }

private:
  CGetter(const CGetter&);
  CGetter& operator=(const CGetter&);

  Getter_f itsGetter_f;
};

/**
 *
 * \c CSetter implements the \c Setter interface by calling a member
 * function of the template class \c C.
 *
 **/
template <class C, class T>
class CSetter : public virtual Setter<T> {
public:
  /// The type of member function pointer that will be called in \c set().
  typedef void (C::* Setter_f) (T);

  /// Construct with a member function pointer.
  CSetter(Setter_f setter) : itsSetter_f(setter) {}

  /// Casts \a item to type \c C* and calls the stored member function.
  virtual void set(void* item, T val) {
	 C* p = static_cast<C*>(item);
	 (p->*itsSetter_f)(val);
  }

private:
  CSetter(const CSetter&);
  CSetter& operator=(const CSetter&);

  Setter_f itsSetter_f;
};

/**
 *
 * \c CGetter implements the \c Attrib interface by calling member
 * functions of the template class \c C.
 *
 **/
template <class C, class T>
class CAttrib : public Attrib<T>, public CGetter<C,T>, public CSetter<C,T> {
public:
  /// The type of member function pointer that will be called in \c get().
  typedef T (C::* Getter_f) () const;

  /// The type of member function pointer that will be called in \c set().
  typedef void (C::* Setter_f) (T);

  /// Construct with a getter and setter member function pointers.
  CAttrib(Getter_f getter, Setter_f setter) :
	 CGetter<C,T>(getter),
	 CSetter<C,T>(setter) {}
};

/**
 *
 * \c CAction implements the \c Action interface by calling a member
 * function of the template class \c C.
 *
 **/
template <class C>
class CAction: public Action {
public:
  /// The type of member function pointer that will be called in \c action().
  typedef void (C::* Action_f) ();

  /// Construct with a member function pointer.
  CAction(Action_f action_f) : itsAction_f(action_f) {}

  /// Casts \a item to type \c C* and calls the stored member function.
  virtual void action(void* item) {
	 C* p = static_cast<C*>(item);
	 (p->*itsAction_f)();
  }

private:
  CAction(const CAction&);
  CAction& operator=(const CAction&);

  Action_f itsAction_f;
};

/**
 *
 * \c CConstAction implements the \c Action interface by calling a
 * const member function of the template class \c C.
 *
 **/
template <class C>
class CConstAction : public Action {
public:
  /// The type of member function pointer that will be called in \c action().
  typedef void (C::* Action_f) () const;

  /// Construct with a member function pointer.
  CConstAction(Action_f action_f) : itsAction_f(action_f) {}

  /// Casts \a item to type \c const \c C* and calls the stored member function.
  virtual void action(void* item) {
	 const C* p = static_cast<C*>(item);
	 (p->*itsAction_f)();
  }

private:
  Action_f itsAction_f;
};


///////////////////////////////////////////////////////////////////////
//
// Begin Tcl namespace ...
//
///////////////////////////////////////////////////////////////////////

namespace Tcl {


///////////////////////////////////////////////////////////////////////
//
// TclItemPkg class definition
//
///////////////////////////////////////////////////////////////////////

class TclItemPkg : public TclItemPkgBase {
public:
  TclItemPkg(Tcl_Interp* interp, const char* name, const char* version,
				 int item_argn=1) :
	 TclItemPkgBase(interp, name, version), 
	 itsItemArgn(item_argn) {}

  virtual void* getItemFromId(int id) = 0;

  int itemArgn() const { return itsItemArgn; }
  
protected:
  template <class T>
  void declareGetter(const char* cmd_name, Getter<T>* getter,
							const char* usage = 0)
  { declareGetter_(this, cmd_name, getter, usage); }
  
  template <class T>
  void declareSetter(const char* cmd_name, Setter<T>* setter,
							const char* usage = 0)
  { declareSetter_(this, cmd_name, setter, usage); }

  template <class T>
  void declareAttrib(const char* attrib_name, Attrib<T>* attrib,
							const char* usage = 0)
  { declareAttrib_(this, attrib_name, attrib, usage); }

  void declareAction(const char* action_name, Action* action,
							const char* usage = 0);

private:
  template <class T>
  static void declareGetter_(TclItemPkg*, const char*, Getter<T>*, const char*);

  template <class T>
  static void declareSetter_(TclItemPkg*, const char*, Setter<T>*, const char*);

  template <class T>
  static void declareAttrib_(TclItemPkg*, const char*, Attrib<T>*, const char*);

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

class TclIoItemPkg : public TclItemPkg {
public:
  TclIoItemPkg(Tcl_Interp* interp, const char* name, const char* version,
					int item_argn=1);

  virtual IO::IoObject& getIoFromId(int id) = 0;
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
  TclItemCmd(const TclItemCmd&);
  TclItemCmd& operator=(const TclItemCmd&);

  TclItemPkg* itsPkg;
};

///////////////////////////////////////////////////////////////////////
//
// CTclItemPkg class definition
//
///////////////////////////////////////////////////////////////////////

template <class C>
class CTclItemPkg : public TclItemPkg {
public:
  CTclItemPkg(Tcl_Interp* interp, const char* name, const char* version,
				  int item_argn=1) :
	 TclItemPkg(interp, name, version, item_argn) {}

  void declareCAction(const char* cmd_name, void (C::* actionFunc) (),
							 const char* usage = 0) {
	 declareAction(cmd_name, new CAction<C>(actionFunc), usage);
  }

  void declareCAction(const char* cmd_name, void (C::* actionFunc) () const,
							 const char* usage = 0) {
	 declareAction(cmd_name, new CConstAction<C>(actionFunc), usage);
  }

  template <class T>
  void declareCGetter(const char* cmd_name, T (C::* getterFunc) () const,
							 const char* usage = 0) {
	 declareGetter(cmd_name, new CGetter<C,T>(getterFunc), usage);
  }

  template <class T>
  void declareCSetter(const char* cmd_name, void (C::* setterFunc) (T),
							 const char* usage = 0) {
	 declareSetter(cmd_name, new CSetter<C,T>(setterFunc), usage);
  }

  template <class T>
  void declareCAttrib(const char* cmd_name,
							 T (C::* getterFunc) () const, void (C::* setterFunc) (T),
							 const char* usage = 0) {
	 declareAttrib(cmd_name, new CAttrib<C,T>(getterFunc, setterFunc), usage);
  }

  void declareProperty(const PropertyInfo<C>& pinfo);

  virtual C* getCItemFromId(int id) = 0;
  virtual void* getItemFromId(int id) {
	 return static_cast<void*>(getCItemFromId(id));
  }
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
	 TclIoItemPkg(interp, name, version, item_argn) 
  {
	 declareGetter("charCount", new CGetter<C, int>(&C::charCount));
  }

  void declareCAction(const char* cmd_name, void (C::* actionFunc) (),
							 const char* usage = 0) {
	 declareAction(cmd_name, new CAction<C>(actionFunc), usage);
  }

  void declareCAction(const char* cmd_name, void (C::* actionFunc) () const,
							 const char* usage = 0) {
	 declareAction(cmd_name, new CConstAction<C>(actionFunc), usage);
  }

  template <class T>
  void declareCGetter(const char* cmd_name, T (C::* getterFunc) () const,
							 const char* usage = 0) {
	 declareGetter(cmd_name, new CGetter<C,T>(getterFunc), usage);
  }

  template <class T>
  void declareCSetter(const char* cmd_name, void (C::* setterFunc) (T),
							 const char* usage = 0) {
	 declareSetter(cmd_name, new CSetter<C,T>(setterFunc), usage);
  }

  template <class T>
  void declareCAttrib(const char* cmd_name,
							 T (C::* getterFunc) () const, void (C::* setterFunc) (T),
							 const char* usage = 0) {
	 declareAttrib(cmd_name, new CAttrib<C,T>(getterFunc, setterFunc), usage);
  }

  void declareProperty(const PropertyInfo<C>& pinfo);
  void declareAllProperties();

  virtual C* getCItemFromId(int id) = 0;
  virtual void* getItemFromId(int id) {
	 return static_cast<void*>(getCItemFromId(id));
  }
};

///////////////////////////////////////////////////////////////////////
//
// CVecPropertyCmd
//
///////////////////////////////////////////////////////////////////////

class VecPropertyCmdBase : public TclCmd {
private:
  VecPropertyCmdBase(const VecPropertyCmdBase&);
  VecPropertyCmdBase& operator=(const VecPropertyCmdBase&);

  TclItemPkg* itsPkg;
  int itsItemArgn;
  int itsValArgn;
  int itsObjcGet;
  int itsObjcSet;

public:
  VecPropertyCmdBase(TclItemPkg* pkg, const char* property_name);

protected:
  virtual const Value& getValFromItemId(int id) = 0;
  virtual void setItemIdToVal(int id, const Value& value) = 0;

  virtual void invoke();
};

template <class C>
class CVecPropertyCmd : public VecPropertyCmdBase {
public:
  CVecPropertyCmd(CTclIoItemPkg<C>* pkg, const PropertyInfo<C>& pinfo) :
	 VecPropertyCmdBase(pkg, pinfo.name_cstr()),
	 itsPkg(pkg),
	 itsPropInfo(pinfo)
  {}

protected:
  virtual const Value& getValFromItemId(int id)
	 {
		C* item = itsPkg->getCItemFromId(id);
		return item->get(itsPropInfo.property());
	 }

  virtual void setItemIdToVal(int id, const Value& value) 
	 {
		C* item = itsPkg->getCItemFromId(id);
		item->set(itsPropInfo.property(), value);
	 }

private:
  CVecPropertyCmd(const CVecPropertyCmd&);
  CVecPropertyCmd& operator=(const CVecPropertyCmd&);

  CTclIoItemPkg<C>* itsPkg;
  const PropertyInfo<C>& itsPropInfo;
};

///////////////////////////////////////////////////////////////////////
//
// CPropertiesCmd
//
///////////////////////////////////////////////////////////////////////

class PropertiesCmdBase : public TclCmd {
private:
  PropertiesCmdBase(const PropertiesCmdBase&);
  PropertiesCmdBase& operator=(const PropertiesCmdBase&);

  Tcl_Interp* itsInterp;
  Tcl_Obj* itsPropertyList;

public:
  PropertiesCmdBase(Tcl_Interp* interp, const char* cmd_name);

protected:
  virtual unsigned int numInfos() = 0;
  virtual const char* getName(unsigned int i) = 0;
  virtual const Value& getMin(unsigned int i) = 0;
  virtual const Value& getMax(unsigned int i) = 0;
  virtual const Value& getRes(unsigned int i) = 0;
  virtual bool getStartNewGroup(unsigned int i) = 0;
  
  virtual void invoke();
};

template <class C>
class CPropertiesCmd : public PropertiesCmdBase {
public:
  CPropertiesCmd(Tcl_Interp* interp, const char* cmd_name) :
	 PropertiesCmdBase(interp, cmd_name) {}

protected:
  virtual unsigned int numInfos() { return C::numPropertyInfos(); }
  virtual const char* getName(unsigned int i)
	 { return C::getPropertyInfo(i).name_cstr(); }
  virtual const Value& getMin(unsigned int i)
	 { return C::getPropertyInfo(i).min(); }
  virtual const Value& getMax(unsigned int i)
	 { return C::getPropertyInfo(i).max(); }
  virtual const Value& getRes(unsigned int i)
	 { return C::getPropertyInfo(i).res(); }
  virtual bool getStartNewGroup(unsigned int i)
	 { return C::getPropertyInfo(i).startNewGroup(); }
};

///////////////////////////////////////////////////////////////////////
//
// CTclIoItemPkg out-of-line member definitions
//
///////////////////////////////////////////////////////////////////////

template <class C>
void CTclIoItemPkg<C>::declareProperty(const PropertyInfo<C>& pinfo) {
  addCommand( new CVecPropertyCmd<C>(this, pinfo) );
}


template <class C>
void CTclIoItemPkg<C>::declareAllProperties() {
  for (unsigned int i = 0; i < C::numPropertyInfos(); ++i) {
	 declareProperty(C::getPropertyInfo(i));
  }

  addCommand( new CPropertiesCmd<C>(TclPkg::interp(),
												makePkgCmdName("properties")));
}


} // end namespace Tcl

static const char vcid_tclitempkg_h[] = "$Header$";
#endif // !TCLITEMPKG_H_DEFINED
