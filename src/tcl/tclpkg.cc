///////////////////////////////////////////////////////////////////////
//
// tclitempkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 12:33:54 1999
// written: Wed Jun 23 14:18:51 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_CC_DEFINED
#define TCLITEMPKG_CC_DEFINED

#include "tclitempkg.h"

#include "tclcmd.h"
#include "stringifycmd.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// TGetterCmd and TSetterCmd classes
//
///////////////////////////////////////////////////////////////////////

template <class T>
class TGetterCmd : public TclCmd {
public:
  TGetterCmd(TclItemPkg* pkg, const char* cmd_name, Getter<T>* getter,
				 int item_argn) :
	 TclCmd(pkg->interp(), cmd_name, 
			  item_argn ? "item_id" : NULL, 
			  item_argn+1, item_argn+1),
	 itsPkg(pkg),
	 itsGetter(getter), 
	 itsItemArgn(item_argn) {}

protected:
  virtual void invoke() {
	 int id = itsItemArgn ? getIntFromArg(itsItemArgn) : -1;
	 void* item = itsPkg->getItemFromId(id);
	 returnVal(itsGetter->get(item));
  }
private:
  TclItemPkg* itsPkg;
  auto_ptr< Getter<T> > itsGetter;
  int itsItemArgn;
};

template <class T>
class TSetterCmd : public TclCmd {
public:
  TSetterCmd(TclItemPkg* pkg, const char* cmd_name, Setter<T>* setter,
				 int item_argn) :
	 TclCmd(pkg->interp(), cmd_name, 
			  item_argn ? "item_id new_value" : "new_value", 
			  item_argn+2, item_argn+2),
	 itsPkg(pkg),
	 itsSetter(setter),
	 itsItemArgn(item_argn),
	 itsValArgn(item_argn+1) {}

protected:
  virtual void invoke() {
	 int id = itsItemArgn ? getIntFromArg(itsItemArgn) : -1;
	 void* item = itsPkg->getItemFromId(id);
	 T val;
	 getValFromArg(itsValArgn, val);
	 itsSetter->set(item, val);
	 returnVoid();
  }
private:
  TclItemPkg* itsPkg;
  auto_ptr< Setter<T> > itsSetter;
  int itsItemArgn;
  int itsValArgn;
};

///////////////////////////////////////////////////////////////////////
//
// TAttribCmd class definition
//
///////////////////////////////////////////////////////////////////////

template <class T>
class TAttribCmd : public TclCmd {
public:
  TAttribCmd(TclItemPkg* pkg, const char* cmd_name,
					 Attrib<T>* attrib, int item_argn) :
	 TclCmd(pkg->interp(), cmd_name,
			  item_argn ? "item_id ?new_value?" : "?new_value?",
			  item_argn+1, item_argn+2),
	 itsPkg(pkg),
	 itsAttrib(attrib),
	 itsItemArgn(item_argn),
	 itsValArgn(item_argn+1), 
	 itsObjcGet(item_argn+1), 
	 itsObjcSet(item_argn+2) {}

protected:
  virtual void invoke() {
	 int id = itsItemArgn ? getIntFromArg(itsItemArgn) : -1;
	 void* item = itsPkg->getItemFromId(id);

	 if (objc() == itsObjcGet) {
		returnVal(itsAttrib->get(item));
	 }
	 else if (objc() == itsObjcSet) {
		T val;
		getValFromArg(itsValArgn, val);
		itsAttrib->set(item, val);
		returnVoid();
	 }
	 else { Assert(0); }
  }

private:
  TclItemPkg* itsPkg;
  auto_ptr< Attrib<T> > itsAttrib;
  int itsItemArgn;
  int itsValArgn;
  int itsObjcGet;
  int itsObjcSet;
};

// We need a specialization of TAttribCmd to handle C++ string's. This
// is because the Attrib itself should deal with 'const string&' for
// efficiency reasons, yet in the set() function we cannot obtain the
// value by passing a reference to getValFromArg() as for the other
// types, since we can't have a reference to a reference.
template <>
class TAttribCmd<const string&> : public TclCmd {
public:
  TAttribCmd(TclItemPkg* pkg, const char* cmd_name,
					 Attrib<const string&>* attrib, int item_argn) :
	 TclCmd(pkg->interp(), cmd_name,
			  item_argn ? "item_id ?new_value?" : "?new_value?",
			  item_argn+1, item_argn+2),
	 itsPkg(pkg),
	 itsAttrib(attrib),
	 itsItemArgn(item_argn),
	 itsValArgn(item_argn+1), 
	 itsObjcGet(item_argn+1), 
	 itsObjcSet(item_argn+2) {}

protected:
  virtual void invoke() {
	 int id = itsItemArgn ? getIntFromArg(itsItemArgn) : -1;
	 void* item = itsPkg->getItemFromId(id);

	 if (objc() == itsObjcGet) {
		returnVal(itsAttrib->get(item));
	 }
	 else if (objc() == itsObjcSet) {
		string val = getStringFromArg(itsValArgn);
		itsAttrib->set(item, val);
		returnVoid();
	 }
	 else { Assert(0); }
  }

private:
  TclItemPkg* itsPkg;
  auto_ptr< Attrib<const string&> > itsAttrib;
  int itsItemArgn;
  int itsValArgn;
  int itsObjcGet;
  int itsObjcSet;
};

///////////////////////////////////////////////////////////////////////
//
// Action command defintion
//
///////////////////////////////////////////////////////////////////////

class ActionCmd : public TclCmd {
public:
  ActionCmd(TclItemPkg* pkg, const char* cmd_name, Action* action,
				int item_argn) :
	 TclCmd(pkg->interp(), cmd_name, 
			  item_argn ? "item_id" : NULL, 
			  item_argn+1, item_argn+1),
	 itsPkg(pkg),
	 itsAction(action),
	 itsItemArgn(item_argn) {}

protected:
  virtual void invoke() {
	 int id = itsItemArgn ? getIntFromArg(itsItemArgn) : -1;
	 void* item = itsPkg->getItemFromId(id);
	 itsAction->action(item);
  }

private:
  TclItemPkg* itsPkg;
  Action* itsAction;
  int itsItemArgn;
};

///////////////////////////////////////////////////////////////////////
//
// TclItemPkg member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
void TclItemPkg::declareGetter_(TclItemPkg* pkg, 
										  const char* cmd_name, Getter<T>* getter) {
  pkg->addCommand( new TGetterCmd<T>(pkg, pkg->makePkgCmdName(cmd_name), 
												 getter, pkg->itsItemArgn) );
}

template <class T>
void TclItemPkg::declareSetter_(TclItemPkg* pkg,
										  const char* cmd_name, Setter<T>* setter) {
  pkg->addCommand( new TSetterCmd<T>(pkg, pkg->makePkgCmdName(cmd_name),
												 setter, pkg->itsItemArgn) );
}

template <class T>
void TclItemPkg::declareAttrib_(TclItemPkg* pkg,
										  const char* attrib_name, Attrib<T>* attrib) {
  pkg->addCommand( new TAttribCmd<T>(pkg, pkg->makePkgCmdName(attrib_name),
												 attrib, pkg->itsItemArgn) );
}

void TclItemPkg::instantiate() {
  Getter<int>* gi=0;             declareGetter_(this, 0, gi);
  Getter<bool>* gb=0;            declareGetter_(this, 0, gb);
  Getter<double>* gd=0;          declareGetter_(this, 0, gd);
  Getter<const char*>* gcc=0;    declareGetter_(this, 0, gcc);
  Getter<const string&>* gcs=0;  declareGetter_(this, 0, gcs);

  Setter<int>* si=0;             declareSetter_(this, 0, si);
  Setter<bool>* sb=0;            declareSetter_(this, 0, sb);
  Setter<double>* sd=0;          declareSetter_(this, 0, sd);
  Setter<const char*>* scc=0;    declareSetter_(this, 0, scc);
//   Setter<const string&>* scs=0;  declareSetter_(this, 0, scs);

  Attrib<int>* ai=0;             declareAttrib_(this, 0, ai);
  Attrib<bool>* ab=0;            declareAttrib_(this, 0, ab);
  Attrib<double>* ad=0;          declareAttrib_(this, 0, ad);
  Attrib<const char*>* acc=0;    declareAttrib_(this, 0, acc);
  Attrib<const string&>* acs=0;  declareAttrib_(this, 0, acs);
}

// void TclItemPkg::declareGetter(const char* cmd_name, Getter<int>* getter) {
//   addCommand( new TGetterCmd<int>(this, makePkgCmdName(cmd_name), 
// 											 getter, itsItemArgn) );
// }
// void TclItemPkg::declareGetter(const char* cmd_name, Getter<bool>* getter) {
//   addCommand( new TGetterCmd<bool>(this, makePkgCmdName(cmd_name),
// 											  getter, itsItemArgn) );
// }
// void TclItemPkg::declareGetter(const char* cmd_name, Getter<double>* getter) {
//   addCommand( new TGetterCmd<double>(this, makePkgCmdName(cmd_name),
// 												 getter, itsItemArgn) );
// }
// void TclItemPkg::declareGetter(const char* cmd_name, 
// 										 Getter<const char*>* getter) {
//   addCommand( new TGetterCmd<const char*>(this, makePkgCmdName(cmd_name),
// 														getter, itsItemArgn) );
// }
// void TclItemPkg::declareGetter(const char* cmd_name, 
// 										 Getter<const string&>* getter) {
//   addCommand( new TGetterCmd<const string&>(this, makePkgCmdName(cmd_name),
// 														  getter, itsItemArgn) );
// }

// void TclItemPkg::declareSetter(const char* cmd_name, Setter<int>* setter) {
//   addCommand( new TSetterCmd<int>(this, makePkgCmdName(cmd_name),
// 											 setter, itsItemArgn) );
// }
// void TclItemPkg::declareSetter(const char* cmd_name, Setter<bool>* setter) {
//   addCommand( new TSetterCmd<bool>(this, makePkgCmdName(cmd_name),
// 											  setter, itsItemArgn) );
// }
// void TclItemPkg::declareSetter(const char* cmd_name, Setter<double>* setter) {
//   addCommand( new TSetterCmd<double>(this, makePkgCmdName(cmd_name),
// 												 setter, itsItemArgn) );
// }
// void TclItemPkg::declareSetter(const char* cmd_name, 
// 										 Setter<const char*>* setter) {
//   addCommand( new TSetterCmd<const char*>(this, makePkgCmdName(cmd_name), 
// 														setter, itsItemArgn) );
// }
// // void TclItemPkg::declareAttrib(const char* attrib_name, 
// // 										 Attrib<const string&>* attrib) {
// //   addCommand( new TAttribCmd<const string&>(
// // 								  this, makePkgCmdName(attrib_name), attrib) );
// // }

// void TclItemPkg::declareAttrib(const char* attrib_name, Attrib<int>* attrib) {
//   addCommand( new TAttribCmd<int>(this, makePkgCmdName(attrib_name),
// 												 attrib, itsItemArgn) );
// }

// void TclItemPkg::declareAttrib(const char* attrib_name, Attrib<bool>* attrib) {
//   addCommand ( new TAttribCmd<bool>(this, makePkgCmdName(attrib_name),
// 													attrib, itsItemArgn) );
// }

// void TclItemPkg::declareAttrib(const char* attrib_name, 
// 										 Attrib<double>* attrib) {
//   addCommand( new TAttribCmd<double>(this, makePkgCmdName(attrib_name),
// 													 attrib, itsItemArgn) );
// }

// void TclItemPkg::declareAttrib(const char* attrib_name, 
// 										 Attrib<const char*>* attrib) {
//   addCommand( new TAttribCmd<const char*>(this, makePkgCmdName(attrib_name),
// 															attrib, itsItemArgn) );
// }

// void TclItemPkg::declareAttrib(const char* attrib_name, 
// 										 Attrib<const string&>* attrib) {
//   addCommand( new TAttribCmd<const string&>(this, makePkgCmdName(attrib_name),
// 														  attrib, itsItemArgn) );
// }

void TclItemPkg::declareAction(const char* action_name, Action* action) {
  addCommand( new ActionCmd(this, makePkgCmdName(action_name),
									 action, itsItemArgn) );
}


///////////////////////////////////////////////////////////////////////
//
// TclIoItemPkg member definitions
//
///////////////////////////////////////////////////////////////////////

class ItemStringifyCmd : public StringifyCmd {
public:
  ItemStringifyCmd(TclIoItemPkg* pkg, int item_argn) :
	 StringifyCmd(pkg->interp(), pkg->makePkgCmdName("stringify"), 
					  item_argn ? "item_id" : NULL,
					  item_argn+1),
	 itsPkg(pkg), 
	 itsItemArgn(item_argn) {}

protected:
  virtual IO& getIO() {
	 int id = itsItemArgn ? getIntFromArg(itsItemArgn) : -1;
	 return itsPkg->getIoFromId(id);
  }
  virtual int getBufSize() { return itsPkg->getBufSize(); }

private:
  TclIoItemPkg* itsPkg;
  int itsItemArgn;
};

class ItemDestringifyCmd : public DestringifyCmd {
public:
  ItemDestringifyCmd(TclIoItemPkg* pkg, int item_argn) :
	 DestringifyCmd(pkg->interp(), pkg->makePkgCmdName("destringify"),
						 item_argn ? "item_id string" : "string", 
						 item_argn+2),
	 itsPkg(pkg),
	 itsItemArgn(item_argn) {}

protected:
  virtual IO& getIO() {
	 int id = itsItemArgn ? getIntFromArg(itsItemArgn) : -1;
	 return itsPkg->getIoFromId(id);
  }

private:
  TclIoItemPkg* itsPkg;
  int itsItemArgn;
};

TclIoItemPkg::TclIoItemPkg(Tcl_Interp* interp, const char* name, 
									const char* version, int item_argn) :
  TclItemPkg(interp, name, version, item_argn)
{
  addCommand( new ItemStringifyCmd(this, itemArgn()) );
  addCommand( new ItemDestringifyCmd(this, itemArgn()) );
}

static const char vcid_tclitempkg_cc[] = "$Header$";
#endif // !TCLITEMPKG_CC_DEFINED
