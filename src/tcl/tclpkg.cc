///////////////////////////////////////////////////////////////////////
//
// tclitempkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 12:33:54 1999
// written: Wed Mar 15 10:17:26 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_CC_DEFINED
#define TCLITEMPKG_CC_DEFINED

#include "tclitempkg.h"

#include "tclcmd.h"
#include "stringifycmd.h"
#include "tclveccmds.h"
#include "util/strings.h"

#include <tcl.h>
#include <vector>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TclItemPkg member definitions
//
///////////////////////////////////////////////////////////////////////

// NOTE: aCC won't compile these static TclItemPkg functions if they
// are defined inside a namespace Tcl {} block. So, as a workaround,
// we must define them at global scope, using explicit Tcl:: qualifiers

template <class T>
void Tcl::TclItemPkg::declareGetter_(TclItemPkg* pkg, const char* cmd_name,
												 Getter<T>* getter, const char* usage) {
  pkg->addCommand(
         new Tcl::TVecGetterCmd<T>(pkg, pkg->makePkgCmdName(cmd_name), 
											  getter, usage, pkg->itsItemArgn)
			);
}

template <class T>
void Tcl::TclItemPkg::declareSetter_(TclItemPkg* pkg, const char* cmd_name,
												 Setter<T>* setter, const char* usage) {
  pkg->addCommand(
		   new Tcl::TVecSetterCmd<T>(pkg, pkg->makePkgCmdName(cmd_name),
											  setter, usage, pkg->itsItemArgn)
			);
}

template <class T>
void Tcl::TclItemPkg::declareAttrib_(TclItemPkg* pkg, const char* attrib_name,
												 Attrib<T>* attrib, const char* usage) {
  pkg->addCommand(
			new Tcl::TVecAttribCmd<T>(pkg, pkg->makePkgCmdName(attrib_name),
											  attrib, usage, pkg->itsItemArgn)
			);
}

void Tcl::TclItemPkg::instantiate() {
  declareGetter_(this, 0, (Getter<int>*) 0, 0);
  declareGetter_(this, 0, (Getter<bool>*) 0, 0);
  declareGetter_(this, 0, (Getter<double>*) 0, 0);
  declareGetter_(this, 0, (Getter<const char*>*) 0, 0);
  declareGetter_(this, 0, (Getter<const string&>*) 0, 0);
  declareGetter_(this, 0, (Getter<const fixed_string&>*) 0, 0);

  declareSetter_(this, 0, (Setter<int>*) 0, 0);
  declareSetter_(this, 0, (Setter<bool>*) 0, 0);
  declareSetter_(this, 0, (Setter<double>*) 0, 0);
  declareSetter_(this, 0, (Setter<const char*>*) 0, 0);
  declareSetter_(this, 0, (Setter<const string&>*) 0, 0);
  declareSetter_(this, 0, (Setter<const fixed_string&>*) 0, 0);

  declareAttrib_(this, 0, (Attrib<int>*) 0, 0);
  declareAttrib_(this, 0, (Attrib<bool>*) 0, 0);
  declareAttrib_(this, 0, (Attrib<double>*) 0, 0);
  declareAttrib_(this, 0, (Attrib<const char*>*) 0, 0);
  declareAttrib_(this, 0, (Attrib<const string&>*) 0, 0);
  declareAttrib_(this, 0, (Attrib<const fixed_string&>*) 0, 0);
}

void Tcl::TclItemPkg::declareAction(const char* action_name, Action* action,
												const char* usage) {
  addCommand( new VecActionCmd(this, makePkgCmdName(action_name),
                               action, usage, itsItemArgn) );
}


///////////////////////////////////////////////////////////////////////
//
// TclIoItemPkg member definitions
//
///////////////////////////////////////////////////////////////////////

namespace Tcl {

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

private:
  ItemStringifyCmd(const ItemStringifyCmd&);
  ItemStringifyCmd& operator=(const ItemStringifyCmd&);

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
  ItemDestringifyCmd(const ItemDestringifyCmd&);
  ItemDestringifyCmd& operator=(const ItemDestringifyCmd&);

  TclIoItemPkg* itsPkg;
  int itsItemArgn;
};

class ItemWriteCmd : public WriteCmd {
public:
  ItemWriteCmd(TclIoItemPkg* pkg, int item_argn) :
	 WriteCmd(pkg->interp(), pkg->makePkgCmdName("write"),
				 item_argn ? "item_id" : NULL,
				 item_argn+1),
	 itsPkg(pkg),
	 itsItemArgn(item_argn) {}

protected:
  virtual IO& getIO() {
	 int id = itsItemArgn ? arg(itsItemArgn).getInt() : -1;
	 return itsPkg->getIoFromId(id);
  }

private:
  ItemWriteCmd(const ItemWriteCmd&);
  ItemWriteCmd& operator=(const ItemWriteCmd&);

  TclIoItemPkg* itsPkg;
  int itsItemArgn;
};

class ItemReadCmd : public ReadCmd {
public:
  ItemReadCmd(TclIoItemPkg* pkg, int item_argn) :
	 ReadCmd(pkg->interp(), pkg->makePkgCmdName("read"),
				 item_argn ? "item_id string" : "string",
				 item_argn+2),
	 itsPkg(pkg),
	 itsItemArgn(item_argn) {}

protected:
  virtual IO& getIO() {
	 int id = itsItemArgn ? arg(itsItemArgn).getInt() : -1;
	 return itsPkg->getIoFromId(id);
  }

private:
  ItemReadCmd(const ItemReadCmd&);
  ItemReadCmd& operator=(const ItemReadCmd&);

  TclIoItemPkg* itsPkg;
  int itsItemArgn;
};

TclIoItemPkg::TclIoItemPkg(Tcl_Interp* interp, const char* name, 
                           const char* version, int item_argn) :
  TclItemPkg(interp, name, version, item_argn)
{
  addCommand( new ItemStringifyCmd(this, itemArgn()) );
  addCommand( new ItemDestringifyCmd(this, itemArgn()) );

  addCommand( new ItemWriteCmd(this, itemArgn()) );
  addCommand( new ItemReadCmd(this, itemArgn()) );
}

} // end namespace Tcl

///////////////////////////////////////////////////////////////////////
//
// VecPropertyCmdBase member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::VecPropertyCmdBase::VecPropertyCmdBase(TclItemPkg* pkg,
														  const char* property_name) :
  TclCmd(pkg->interp(), pkg->makePkgCmdName(property_name),
			(pkg->itemArgn() ? "item_id(s) ?new_value(s)?" : "?new_value?"),
			pkg->itemArgn()+1, pkg->itemArgn()+2, false),
  itsPkg(pkg),
  itsItemArgn(pkg->itemArgn()),
  itsValArgn(pkg->itemArgn()+1),
  itsObjcGet(pkg->itemArgn()+1),
  itsObjcSet(pkg->itemArgn()+2)
{
DOTRACE("Tcl::VecPropertyCmdBase::VecPropertyCmdBase");
}

void Tcl::VecPropertyCmdBase::invoke() {
DOTRACE("Tcl::VecPropertyCmdBase::invoke");
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
	 returnVal( getValFromItemId(ids[0]) );

	 for (size_t i = 1; i < ids.size(); ++i) {
		lappendVal( getValFromItemId(ids[i]) );
	 }

  }
  // ... or if we are setting
  else if (TclCmd::objc() == itsObjcSet) {
	 vector<TclValue> vals;

	 if (ids.size() == 1) {
		vals.push_back(TclValue(arg(itsValArgn)));
	 }
	 else {
		getValSequenceFromArg(itsValArgn, back_inserter(vals));
	 }

	 size_t max_valn = vals.size()-1;

	 for (size_t i = 0, valn = 0; i < ids.size(); ++i) {
		setItemIdToVal(ids[i], vals[valn]);
		if (valn < max_valn) ++valn;
	 }
  }
  // ... or ... "can't happen"
  else { /* Assert(0); */ }
}

///////////////////////////////////////////////////////////////////////
//
// PropertiesCmdBase member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::PropertiesCmdBase::PropertiesCmdBase(Tcl_Interp* interp, 
														const char* cmd_name) :
  TclCmd(interp, cmd_name, NULL, 1, 1),
  itsInterp(interp),
  itsPropertyList(0)
{
DOTRACE("Tcl::PropertiesCmdBase::PropertiesCmdBase");
}

void Tcl::PropertiesCmdBase::invoke() {
DOTRACE("Tcl::PropertiesCmdBase::invoke");
  if (itsPropertyList == 0) {

	 vector<Tcl_Obj*> elements;
		
	 unsigned int num_infos = numInfos();

	 for (size_t i = 0; i < num_infos; ++i) {
		vector<Tcl_Obj*> sub_elements;
		  
		// property name
		sub_elements.push_back(Tcl_NewStringObj(getName(i), -1));
		  
		// min value
		TclValue min(itsInterp, getMin(i));
		sub_elements.push_back(min.getObj());
		  
		// max value
		TclValue max(itsInterp, getMax(i));
		sub_elements.push_back(max.getObj());
		  
		// resolution value
		TclValue res(itsInterp, getRes(i));
		sub_elements.push_back(res.getObj());
		  
		// start new group flag
		sub_elements.push_back(Tcl_NewBooleanObj(getStartNewGroup(i)));
		  
		elements.push_back(Tcl_NewListObj(sub_elements.size(),
													 &(sub_elements[0])));
	 }

	 itsPropertyList = Tcl_NewListObj(elements.size(), &(elements[0]));

	 Tcl_IncrRefCount(itsPropertyList);
  }

  returnVal(TclValue(itsInterp, itsPropertyList));
}

#ifdef ACC_COMPILER
void do_nothing() { 
  typeid(bad_alloc);
  typeid(logic_error);
  typeid(length_error);
  typeid(out_of_range);
}
#endif

static const char vcid_tclitempkg_cc[] = "$Header$";
#endif // !TCLITEMPKG_CC_DEFINED
