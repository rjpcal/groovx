///////////////////////////////////////////////////////////////////////
//
// tclitempkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 12:33:54 1999
// written: Wed Dec  8 00:16:15 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_CC_DEFINED
#define TCLITEMPKG_CC_DEFINED

#include "tclitempkg.h"

#include <string>

#include "tclcmd.h"
#include "stringifycmd.h"
#include "tclveccmds.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace Tcl {

///////////////////////////////////////////////////////////////////////
//
// TGetterCmd class definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
class TGetterCmd : public virtual TclCmd {
public:
  TGetterCmd(TclItemPkg* pkg, const char* cmd_name, Getter<T>* getter,
             const char* usage, int item_argn) :
    TclCmd(pkg->interp(), cmd_name, 
           usage ? usage : (item_argn ? "item_id" : NULL), 
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

///////////////////////////////////////////////////////////////////////
//
// TSetterCmd class definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
class TSetterCmd : public virtual TclCmd {
public:
  TSetterCmd(TclItemPkg* pkg, const char* cmd_name, Setter<T>* setter,
             const char* usage, int item_argn) :
    TclCmd(pkg->interp(), cmd_name, 
           usage ? usage : (item_argn ? "item_id new_value" : "new_value"), 
           item_argn+2, item_argn+2),
    itsPkg(pkg),
    itsSetter(setter),
    itsItemArgn(item_argn),
    itsValArgn(item_argn+1) {}

protected:
  virtual void invoke() {
    int id = itsItemArgn ? getIntFromArg(itsItemArgn) : -1;
    void* item = itsPkg->getItemFromId(id);
    set(item);
    returnVoid();
  }

private:
  void set(void* item) {
    T val;
    getValFromArg(itsValArgn, val);
    itsSetter->set(item, val);
  }

  TclItemPkg* itsPkg;
  auto_ptr< Setter<T> > itsSetter;
  int itsItemArgn;
  int itsValArgn;
};

// Specialization for T=const string&, since we must declare 'val' as
// type 'string' rather than 'const string&'.
template<>
void TSetterCmd<const string&>::set(void* item) {
  string val = getStringFromArg(itsValArgn);
  itsSetter->set(item, val);
}

///////////////////////////////////////////////////////////////////////
//
// TAttribCmd class definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
class TAttribCmd : public TGetterCmd<T>, public TSetterCmd<T> {
public:
  TAttribCmd(TclItemPkg* pkg, const char* cmd_name, Attrib<T>* attrib,
             const char* usage, int item_argn) :
    TGetterCmd<T>(pkg, 0, attrib, 0, item_argn),
    TSetterCmd<T>(pkg, 0, attrib, 0, item_argn),
    TclCmd(pkg->interp(), cmd_name,
           usage ? usage : (item_argn ? "item_id ?new_value?" : "?new_value?"),
           item_argn+1, item_argn+2),
    itsObjcGet(item_argn+1), 
    itsObjcSet(item_argn+2) {}

protected:

  virtual void invoke() {
    if      (TclCmd::objc() == itsObjcGet) { TGetterCmd<T>::invoke(); }
    else if (TclCmd::objc() == itsObjcSet) { TSetterCmd<T>::invoke(); }
    else         /* "can't happen */       { Assert(0); }
  }

private:
  int itsObjcGet;
  int itsObjcSet;
};

///////////////////////////////////////////////////////////////////////
//
// ActionCmd class defintions
//
///////////////////////////////////////////////////////////////////////

class ActionCmd : public TclCmd {
public:
  ActionCmd(TclItemPkg* pkg, const char* cmd_name, Action* action,
            const char* usage, int item_argn) :
    TclCmd(pkg->interp(), cmd_name, 
           usage ? usage : (item_argn ? "item_id" : NULL),
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

} // end namespace Tcl

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
  pkg->addCommand( new Tcl::TVecGetterCmd<T>(pkg, pkg->makePkgCmdName(cmd_name), 
															getter, usage, pkg->itsItemArgn) );
}

template <class T>
void Tcl::TclItemPkg::declareSetter_(TclItemPkg* pkg, const char* cmd_name,
												 Setter<T>* setter, const char* usage) {
  pkg->addCommand( new Tcl::TVecSetterCmd<T>(pkg, pkg->makePkgCmdName(cmd_name),
															setter, usage, pkg->itsItemArgn) );
}

template <class T>
void Tcl::TclItemPkg::declareAttrib_(TclItemPkg* pkg, const char* attrib_name,
												 Attrib<T>* attrib, const char* usage) {
  pkg->addCommand( new Tcl::TVecAttribCmd<T>(pkg, pkg->makePkgCmdName(attrib_name),
															attrib, usage, pkg->itsItemArgn) );
}

namespace Tcl {

void TclItemPkg::instantiate() {
  Getter<int>* gi=0;             declareGetter_(this, 0, gi, 0);
  Getter<bool>* gb=0;            declareGetter_(this, 0, gb, 0);
  Getter<double>* gd=0;          declareGetter_(this, 0, gd, 0);
  Getter<const char*>* gcc=0;    declareGetter_(this, 0, gcc, 0);
  Getter<const string&>* gcs=0;  declareGetter_(this, 0, gcs, 0);

  Setter<int>* si=0;             declareSetter_(this, 0, si, 0);
  Setter<bool>* sb=0;            declareSetter_(this, 0, sb, 0);
  Setter<double>* sd=0;          declareSetter_(this, 0, sd, 0);
  Setter<const char*>* scc=0;    declareSetter_(this, 0, scc, 0);
  Setter<const string&>* scs=0;  declareSetter_(this, 0, scs, 0);

  Attrib<int>* ai=0;             declareAttrib_(this, 0, ai, 0);
  Attrib<bool>* ab=0;            declareAttrib_(this, 0, ab, 0);
  Attrib<double>* ad=0;          declareAttrib_(this, 0, ad, 0);
  Attrib<const char*>* acc=0;    declareAttrib_(this, 0, acc, 0);
  Attrib<const string&>* acs=0;  declareAttrib_(this, 0, acs, 0);
}

void TclItemPkg::declareAction(const char* action_name, Action* action,
                               const char* usage) {
  addCommand( new VecActionCmd(this, makePkgCmdName(action_name),
                               action, usage, itsItemArgn) );
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

static const char vcid_tclitempkg_cc[] = "$Header$";
#endif // !TCLITEMPKG_CC_DEFINED
