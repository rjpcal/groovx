///////////////////////////////////////////////////////////////////////
//
// tclitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 12:33:59 1999
// written: Mon Oct  4 12:29:30 1999
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

#ifndef PROPERTY_H_DEFINED
#include "property.h"
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

class IO;

class TclIoItemPkg : public TclItemPkg {
public:
  TclIoItemPkg(Tcl_Interp* interp, const char* name, const char* version,
					int item_argn=1);

  virtual IO& getIoFromId(int id) = 0;
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

template <class C>
class CVecPropertyCmd : public TclCmd {
public:
  CVecPropertyCmd(CTclIoItemPkg<C>* pkg, const PropertyInfo<C>& pinfo) :
	 TclCmd(pkg->interp(), pkg->makePkgCmdName(pinfo.name.c_str()),
			  (pkg->itemArgn() ? "item_id(s) ?new_value(s)?" : "?new_value?"),
			  pkg->itemArgn()+1, pkg->itemArgn()+2, false),
	 itsPkg(pkg),
	 itsPropInfo(pinfo),
	 itsItemArgn(pkg->itemArgn()),
	 itsValArgn(pkg->itemArgn()+1),
	 itsObjcGet(pkg->itemArgn()+1),
	 itsObjcSet(pkg->itemArgn()+2)
  {}

protected:
  virtual void invoke() {
	 // Fetch the item ids
	 vector<int> ids;

	 if (itsItemArgn) {
		getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
	 }
	 else {
		// -1 is the cue to use the default item
		ids.push_back(-1);
	 }

	 // If we are getting...
	 if (TclCmd::objc() == itsObjcGet) {
		C* item = itsPkg->getCItemFromId(ids[0]);
		returnVal( item->get(itsPropInfo.property) );
		
		for (int i = 1; i < ids.size(); ++i) {
		  C* item = itsPkg->getCItemFromId(ids[i]);
		  lappendVal( item->get(itsPropInfo.property) );
		}

	 }
	 // ... or if we are setting
	 else if (TclCmd::objc() == itsObjcSet) {
		vector<TclValue> vals;

		if (ids.size() == 1) {
 		  vals.push_back(TclValue(itsPkg->interp(), arg(itsValArgn)));
		}
		else {
 		  getValSequenceFromArg(itsValArgn, back_inserter(vals));
		}

		int max_valn = vals.size()-1;

		for (int i = 0, valn = 0; i < ids.size(); ++i) {
		  C* item = itsPkg->getCItemFromId(ids[i]);
 		  item->set(itsPropInfo.property, vals[valn]);
		  if (valn < max_valn) ++valn;
		}
	 }
	 // ... or ... "can't happen"
	 else { /* Assert(0); */ }
  }

private:
  CTclIoItemPkg<C>* itsPkg;
  const PropertyInfo<C>& itsPropInfo;
  int itsItemArgn;
  int itsValArgn;
  int itsObjcGet;
  int itsObjcSet;
};

///////////////////////////////////////////////////////////////////////
//
// CPropertiesCmd
//
///////////////////////////////////////////////////////////////////////

template <class C>
class CPropertiesCmd : public TclCmd {
public:
  CPropertiesCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1),
	 itsInterp(interp),
	 itsPropertyList(0) {}
protected:
  virtual void invoke() {
	 if (itsPropertyList == 0) {

		const vector<PropertyInfo<C> >& vec = C::getPropertyInfos();
		
		vector<Tcl_Obj*> elements;
		
		for (int i = 0; i < vec.size(); ++i) {
		  vector<Tcl_Obj*> sub_elements;
		  
		  // property name
		  sub_elements.push_back(Tcl_NewStringObj(vec[i].name.c_str(), -1));
		  
		  // min value
		  TclValue min(itsInterp, *(vec[i].min));
		  sub_elements.push_back(min.getObj());
		  
		  // max value
		  TclValue max(itsInterp, *(vec[i].max));
		  sub_elements.push_back(max.getObj());
		  
		  // resolution value
		  TclValue res(itsInterp, *(vec[i].res));
		  sub_elements.push_back(res.getObj());
		  
		  // start new group flag
		  sub_elements.push_back(Tcl_NewBooleanObj(vec[i].startNewGroup));
		  
		  elements.push_back(Tcl_NewListObj(sub_elements.size(),
														&(sub_elements[0])));
		}
		
		itsPropertyList = Tcl_NewListObj(elements.size(),
											 &(elements[0]));
		
		Tcl_IncrRefCount(itsPropertyList);
	 }
	 
	 returnVal(TclValue(itsInterp, itsPropertyList));
  }

private:
  Tcl_Interp* itsInterp;
  Tcl_Obj* itsPropertyList;
};

///////////////////////////////////////////////////////////////////////
//
// CTclIoItemPkg definitions
//
///////////////////////////////////////////////////////////////////////

template <class C>
void CTclIoItemPkg::declareProperty(const PropertyInfo<C>& pinfo) {
  addCommand( new CVecPropertyCmd<C>(this, pinfo) );
}
  
template <class C>
void CTclIoItemPkg::declareAllProperties() {
  const vector<PropertyInfo<C> >& pinfos = C::getPropertyInfos();

  for (int i = 0; i < pinfos.size(); ++i) {
	 declareProperty(pinfos[i]);
  }

  addCommand( new CPropertiesCmd<C>(TclPkg::interp(),
												makePkgCmdName("properties")));
}

static const char vcid_tclitempkg_h[] = "$Header$";
#endif // !TCLITEMPKG_H_DEFINED
