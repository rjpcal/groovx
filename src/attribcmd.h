///////////////////////////////////////////////////////////////////////
//
// attribcmd.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 19:49:07 1999
// written: Tue Jun 15 12:38:26 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ATTRIBCMD_H_DEFINED
#define ATTRIBCMD_H_DEFINED

#ifndef TCLCMD_H_DEFINED
#include "tclcmd.h"
#endif

template <class C, class T>
class TclGetterCmd : public TclCmd {
public:
  typedef T (C::* Getter) () const;

  TclGetterCmd(Tcl_Interp* interp, const char* cmd_name, Getter getter) :
	 TclCmd(interp, cmd_name, "item_id", 2, 2),
	 itsGetter(getter) {}

protected:
  virtual C* getItemFromId(int id) = 0;

  virtual void invoke() {
	 int id = getIntFromArg(1);
	 const C* p = getItemFromId(id);
	 T val = (p->*itsGetter)();
	 returnVal(val);
  }

private:
  Getter itsGetter;
};

template <class C, class T>
class TclSetterCmd : public TclCmd {
public:
  typedef void (C::* Setter) (T);

  TclSetterCmd(Tcl_Interp* interp, const char* cmd_name, Setter setter) :
	 TclCmd(interp, cmd_name, "item_id new_value", 3, 3),
	 itsSetter(setter) {}

protected:
  virtual C* getItemFromId(int id) = 0;

  virtual void invoke() {
	 int id = getIntFromArg(1);
	 C* p = getItemFromId(id);
	 T val;
	 getValFromArg(2, val);
	 (p->*itsSetter)(val);
	 returnVoid();
  }

private:
  Setter itsSetter;
};

template <class C, class T>
class TclAttribCmd : public TclCmd {
public:
  typedef T (C::* Getter) () const;
  typedef void (C::* Setter) (T);

  TclAttribCmd(Tcl_Interp* interp, const char* cmd_name,
					Getter getter, Setter setter) :
	 TclCmd(interp, cmd_name, "item_id ?new_value?", 2, 3),
	 itsGetter(getter), 
	 itsSetter(setter) {}

protected:
  virtual C* getItemFromId(int id) = 0;

  virtual void invoke() {
	 int id = getIntFromArg(1);
	 C* p = getItemFromId(id);
	 if (objc() == 2) {
		T val = (p->*itsGetter)();
		returnVal(val);
	 }
	 else if (objc() == 3) {
		T val;
		getValFromArg(2, val);
		(p->*itsSetter)(val);
		returnVoid();
	 }
  }

private:
  Getter itsGetter;
  Setter itsSetter;
};
	 
static const char vcid_attribcmd_h[] = "$Header$";
#endif // !ATTRIBCMD_H_DEFINED
