///////////////////////////////////////////////////////////////////////
//
// tclitempkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 12:33:54 1999
// written: Wed Nov  1 17:45:37 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_CC_DEFINED
#define TCLITEMPKG_CC_DEFINED

#include "tcl/tclitempkg.h"

#include "tcl/stringifycmd.h"
#include "tcl/tclcmd.h"
#include "tcl/tclveccmds.h"

#include "util/arrays.h"
#include "util/strings.h"

#include <tcl.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TclItemPkg member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TclItemPkg::TclItemPkg(Tcl_Interp* interp,
									 const char* name, const char* version,
									 int item_argn) :
  TclItemPkgBase(interp, name, version), 
  itsItemArgn(item_argn)
{}

Tcl::TclItemPkg::~TclItemPkg() {}

// NOTE: aCC won't compile these TclItemPkg functions if they are
// defined inside a namespace Tcl {} block. So, as a workaround, we
// must define them at global scope, using explicit Tcl:: qualifiers

template <class T>
void Tcl::TclItemPkg::declareGetter(const char* cmd_name,
												Getter<T>* getter, const char* usage) {
  addCommand(
         new Tcl::TVecGetterCmd<T>(this, makePkgCmdName(cmd_name), 
											  getter, usage, itsItemArgn)
			);
}

template <class T>
void Tcl::TclItemPkg::declareSetter(const char* cmd_name,
												Setter<T>* setter, const char* usage) {
  addCommand(
		   new Tcl::TVecSetterCmd<T>(this, makePkgCmdName(cmd_name),
											  setter, usage, itsItemArgn)
			);
}

template <class T>
void Tcl::TclItemPkg::declareAttrib(const char* attrib_name,
												Attrib<T>* attrib, const char* usage) {
  addCommand(
			new Tcl::TVecAttribCmd<T>(this, makePkgCmdName(attrib_name),
											  attrib, usage, itsItemArgn)
			);
}

void Tcl::TclItemPkg::instantiate() {
  declareGetter(0, (Getter<int>*) 0, 0);
  declareGetter(0, (Getter<bool>*) 0, 0);
  declareGetter(0, (Getter<double>*) 0, 0);
  declareGetter(0, (Getter<const char*>*) 0, 0);
  declareGetter(0, (Getter<const fixed_string&>*) 0, 0);

  declareSetter(0, (Setter<int>*) 0, 0);
  declareSetter(0, (Setter<bool>*) 0, 0);
  declareSetter(0, (Setter<double>*) 0, 0);
  declareSetter(0, (Setter<const char*>*) 0, 0);
  declareSetter(0, (Setter<const fixed_string&>*) 0, 0);

  declareAttrib(0, (Attrib<int>*) 0, 0);
  declareAttrib(0, (Attrib<bool>*) 0, 0);
  declareAttrib(0, (Attrib<double>*) 0, 0);
  declareAttrib(0, (Attrib<const char*>*) 0, 0);
  declareAttrib(0, (Attrib<const fixed_string&>*) 0, 0);
}

void Tcl::TclItemPkg::declareAction(const char* action_name, Action* action,
												const char* usage) {
  addCommand( new VecActionCmd(this, makePkgCmdName(action_name),
                               action, usage, itsItemArgn) );
}


///////////////////////////////////////////////////////////////////////
//
// IoFetcher member definitions and IO commands
//
///////////////////////////////////////////////////////////////////////

Tcl::IoFetcher::IoFetcher() {
DOTRACE("Tcl::IoFetcher::IoFetcher");
}

Tcl::IoFetcher::~IoFetcher() {
DOTRACE("Tcl::IoFetcher::~IoFetcher");
}

namespace Tcl {

class ItemStringifyCmd : public StringifyCmd {
public:
  ItemStringifyCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
    StringifyCmd(pkg->interp(), pkg->makePkgCmdName("stringify"), 
                 item_argn ? "item_id" : NULL,
                 item_argn+1),
    itsFetcher(fetcher), 
    itsItemArgn(item_argn) {}

protected:
  virtual IO::IoObject& getIO() {
    int id = itsItemArgn ? getIntFromArg(itsItemArgn) : -1;
    return itsFetcher->getIoFromId(id);
  }

private:
  ItemStringifyCmd(const ItemStringifyCmd&);
  ItemStringifyCmd& operator=(const ItemStringifyCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

class ItemDestringifyCmd : public DestringifyCmd {
public:
  ItemDestringifyCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
    DestringifyCmd(pkg->interp(), pkg->makePkgCmdName("destringify"),
                   item_argn ? "item_id string" : "string", 
                   item_argn+2),
    itsFetcher(fetcher),
    itsItemArgn(item_argn) {}

protected:
  virtual IO::IoObject& getIO() {
    int id = itsItemArgn ? getIntFromArg(itsItemArgn) : -1;
    return itsFetcher->getIoFromId(id);
  }

private:
  ItemDestringifyCmd(const ItemDestringifyCmd&);
  ItemDestringifyCmd& operator=(const ItemDestringifyCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

class ItemWriteCmd : public WriteCmd {
public:
  ItemWriteCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
	 WriteCmd(pkg->interp(), pkg->makePkgCmdName("write"),
				 item_argn ? "item_id" : NULL,
				 item_argn+1),
	 itsFetcher(fetcher),
	 itsItemArgn(item_argn) {}

protected:
  virtual IO::IoObject& getIO() {
	 int id = itsItemArgn ? arg(itsItemArgn).getInt() : -1;
	 return itsFetcher->getIoFromId(id);
  }

private:
  ItemWriteCmd(const ItemWriteCmd&);
  ItemWriteCmd& operator=(const ItemWriteCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

class ItemReadCmd : public ReadCmd {
public:
  ItemReadCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
	 ReadCmd(pkg->interp(), pkg->makePkgCmdName("read"),
				 item_argn ? "item_id string" : "string",
				 item_argn+2),
	 itsFetcher(fetcher),
	 itsItemArgn(item_argn) {}

protected:
  virtual IO::IoObject& getIO() {
	 int id = itsItemArgn ? arg(itsItemArgn).getInt() : -1;
	 return itsFetcher->getIoFromId(id);
  }

private:
  ItemReadCmd(const ItemReadCmd&);
  ItemReadCmd& operator=(const ItemReadCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

class ItemASRLoadCmd : public ASRLoadCmd {
public:
  ItemASRLoadCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
	 ASRLoadCmd(pkg->interp(), pkg->makePkgCmdName("load"),
					item_argn ? "item_id filename" : "filename",
					item_argn+2),
	 itsFetcher(fetcher),
	 itsItemArgn(item_argn) {}
  
protected:
  virtual IO::IoObject& getIO() {
	 int id = itsItemArgn ? arg(itsItemArgn).getInt() : -1;
	 return itsFetcher->getIoFromId(id);
  }

  virtual const char* getFilename() {
	 return getCstringFromArg(itsItemArgn+1);
  }

private:
  ItemASRLoadCmd(const ItemASRLoadCmd&);
  ItemASRLoadCmd& operator=(const ItemASRLoadCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

class ItemASWSaveCmd : public ASWSaveCmd {
public:
  ItemASWSaveCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
	 ASWSaveCmd(pkg->interp(), pkg->makePkgCmdName("save"),
					item_argn ? "item_id filename" : "filename",
					item_argn+2),
	 itsFetcher(fetcher),
	 itsItemArgn(item_argn) {}
  
protected:
  virtual IO::IoObject& getIO() {
	 int id = itsItemArgn ? arg(itsItemArgn).getInt() : -1;
	 return itsFetcher->getIoFromId(id);
  }

  virtual const char* getFilename() {
	 return getCstringFromArg(itsItemArgn+1);
  }

private:
  ItemASWSaveCmd(const ItemASWSaveCmd&);
  ItemASWSaveCmd& operator=(const ItemASWSaveCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

} // end namespace Tcl

void Tcl::TclItemPkg::addIoCommands(IoFetcher* fetcher) {
DOTRACE("Tcl::TclItemPkg::addIoCommands");
  addCommand( new ItemStringifyCmd(this, fetcher, itemArgn()) );
  addCommand( new ItemDestringifyCmd(this, fetcher, itemArgn()) );

  addCommand( new ItemWriteCmd(this, fetcher, itemArgn()) );
  addCommand( new ItemReadCmd(this, fetcher, itemArgn()) );

  addCommand( new ItemASWSaveCmd(this, fetcher, itemArgn()) );
  addCommand( new ItemASRLoadCmd(this, fetcher, itemArgn()) );
}

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

Tcl::VecPropertyCmdBase::~VecPropertyCmdBase() {}

void Tcl::VecPropertyCmdBase::invoke() {
DOTRACE("Tcl::VecPropertyCmdBase::invoke");
  // Fetch the item ids
  dynamic_block<int> ids(1); 

  if (itsItemArgn) {
	 unsigned int num_ids = getSequenceLengthOfArg(itsItemArgn);
	 DebugEvalNL(num_ids);
	 ids.resize(num_ids);
	 getSequenceFromArg(itsItemArgn, &ids[0], (int*) 0);
  }
  else {
	 // -1 is the cue to use the default item
	 ids.at(0) = -1;
  }

  // If we are getting...
  if (TclCmd::objc() == itsObjcGet) {
	 if ( ids.size() == 0 )
		/* return an empty Tcl result since the list of ids was empty */
		return;
	 else if ( ids.size() == 1 )
		returnVal( getValFromItemId(ids[0]) );
	 else
		for (size_t i = 0; i < ids.size(); ++i) {
		  lappendVal( getValFromItemId(ids[i]) );
		}

  }
  // ... or if we are setting
  else if (TclCmd::objc() == itsObjcSet) {

	 Tcl::ListIterator<Tcl::TclValue>
		val_itr = beginOfArg(itsValArgn, (TclValue*)0),
		val_end = endOfArg(itsValArgn, (TclValue*)0);

	 TclValue val = *val_itr;

	 for (size_t i = 0; i < ids.size(); ++i) {
		setItemIdToVal(ids[i], val);

		// Only fetch a new value if there are more values to get... if
		// we run out of values before we run out of ids, then we just
		// keep on using the last value in the sequence of values.
		if (val_itr != val_end)
		  if (++val_itr != val_end)
			 val = *val_itr;
	 }
  }
  // ... or ... "can't happen"
  else {  Assert(0);  }
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

Tcl::PropertiesCmdBase::~PropertiesCmdBase() {}

void Tcl::PropertiesCmdBase::invoke() {
DOTRACE("Tcl::PropertiesCmdBase::invoke");
  if (itsPropertyList == 0) {

	 unsigned int num_infos = numInfos();

	 fixed_block<Tcl_Obj*> elements(num_infos);

	 for (size_t i = 0; i < num_infos; ++i) {
		fixed_block<Tcl_Obj*> sub_elements(5);
		  
		// property name
		sub_elements.at(0) = Tcl_NewStringObj(getName(i), -1);
		  
		// min value
		TclValue min(itsInterp, getMin(i));
		sub_elements.at(1) = min.getObj();
		  
		// max value
		TclValue max(itsInterp, getMax(i));
		sub_elements.at(2) = max.getObj();
		  
		// resolution value
		TclValue res(itsInterp, getRes(i));
		sub_elements.at(3) = res.getObj();
		  
		// start new group flag
		sub_elements.at(4) = Tcl_NewBooleanObj(getStartNewGroup(i));
		  
 		elements.at(i) = Tcl_NewListObj(sub_elements.size(),
												  &(sub_elements[0]));
	 }

	 itsPropertyList = Tcl_NewListObj(elements.size(), &(elements[0]));

	 Tcl_IncrRefCount(itsPropertyList);
  }

  returnVal(TclValue(itsInterp, itsPropertyList));
}

static const char vcid_tclitempkg_cc[] = "$Header$";
#endif // !TCLITEMPKG_CC_DEFINED
