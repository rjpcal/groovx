///////////////////////////////////////////////////////////////////////
//
// tclitempkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 12:33:54 1999
// written: Tue Nov 30 17:34:55 1999
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
// TGetterCmd and TVecGetterCmd class definitions
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

template <class ValType>
class TVecGetterCmd : public virtual TclCmd {
public:
  TVecGetterCmd(TclItemPkg* pkg, const char* cmd_name, Getter<ValType>* getter,
                const char* usage, int item_argn) :
    TclCmd(pkg->interp(), cmd_name, 
           usage ? usage : (item_argn ? "item_id(s)" : NULL), 
           item_argn+1, item_argn+1),
    itsPkg(pkg),
    itsGetter(getter), 
    itsItemArgn(item_argn) {}

protected:
  virtual void invoke() {
    vector<int> ids;
    if (itsItemArgn) {
      getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
    }
    else {
      ids.push_back(-1);
    }

    void* item = itsPkg->getItemFromId(ids[0]);
    returnVal(itsGetter->get(item));

    for (size_t i = 1; i < ids.size(); ++i) {
      void* item = itsPkg->getItemFromId(ids[i]);
      lappendVal(itsGetter->get(item));
    }
  }
private:
  TclItemPkg* itsPkg;
  auto_ptr< Getter<ValType> > itsGetter;
  int itsItemArgn;
};

template<>
void TVecGetterCmd<const string&>::invoke() {
  int id = itsItemArgn ? getIntFromArg(itsItemArgn) : -1;
  void* item = itsPkg->getItemFromId(id);
  returnVal(itsGetter->get(item));
}

///////////////////////////////////////////////////////////////////////
//
// TSetterCmd and TVecSetterCmd class definitions
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

template <class T>
class TVecSetterCmd : public virtual TclCmd {
public:
  TVecSetterCmd(TclItemPkg* pkg, const char* cmd_name, Setter<T>* setter,
                const char* usage, int item_argn) :
    TclCmd(pkg->interp(), cmd_name, 
           usage ? usage : (item_argn ? 
                            "item_id(s) new_value(s)" : "new_value"), 
           item_argn+2, item_argn+2),
    itsPkg(pkg),
    itsSetter(setter),
    itsItemArgn(item_argn),
    itsValArgn(item_argn+1) {}

protected:
  virtual void invoke() {
    vector<int> ids;
    if (itsItemArgn) {
      getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
    }
    else {
      ids.push_back(-1);
    }

    vector<T> vals;
	 if (ids.size() == 1) {
		T val;
		getValFromArg(itsValArgn, val);
		vals.push_back(val);
	 }
	 else {
		getSequenceFromArg(itsValArgn, back_inserter(vals), (T*) 0);
	 }

    int max_valn = vals.size()-1;

    for (size_t i = 0, valn = 0; i < ids.size(); ++i) {
      void* item = itsPkg->getItemFromId(ids[i]);
      itsSetter->set(item, vals[valn]);
      if (valn < max_valn) ++valn;
    }

    returnVoid();
  }

private:
  TclItemPkg* itsPkg;
  auto_ptr< Setter<T> > itsSetter;
  int itsItemArgn;
  int itsValArgn;
};

// Specialization for T=const string&, since we must declare 'vals' as
// type 'vector<string>' rather than 'vector<const string&>'.
template<>
void TVecSetterCmd<const string&>::invoke() {
    vector<int> ids;
    if (itsItemArgn) {
      getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
    }
    else {
      ids.push_back(-1);
    }

    vector<string> vals;
	 if (ids.size() == 1) {
		string val;
		getValFromArg(itsValArgn, val);
		vals.push_back(val);
	 }
	 else {
		getSequenceFromArg(itsValArgn, back_inserter(vals), (string*) 0);
	 }

    int max_valn = vals.size()-1;

    for (size_t i = 0, valn = 0; i < ids.size(); ++i) {
      void* item = itsPkg->getItemFromId(ids[i]);
      itsSetter->set(item, vals[valn]);
      if (valn < max_valn) ++valn;
    }

    returnVoid();
}

///////////////////////////////////////////////////////////////////////
//
// TAttribCmd and TVecAttrib class definitions
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

template <class T>
class TVecAttribCmd : public TVecGetterCmd<T>, public TVecSetterCmd<T> {
public:
  TVecAttribCmd(TclItemPkg* pkg, const char* cmd_name, Attrib<T>* attrib,
                const char* usage, int item_argn) :
    TVecGetterCmd<T>(pkg, 0, attrib, 0, item_argn),
    TVecSetterCmd<T>(pkg, 0, attrib, 0, item_argn),
    TclCmd(pkg->interp(), cmd_name,
           usage ? usage : (item_argn ?
                            "item_id(s) ?new_value(s)?" : "?new_value?"),
           item_argn+1, item_argn+2, false),
    itsObjcGet(item_argn+1),
    itsObjcSet(item_argn+2) {}

  ~TVecAttribCmd() { DOTRACE("TVecAttribCmd::~TVecAttribCmd"); }

protected:
  virtual void invoke() {
    if      (TclCmd::objc() == itsObjcGet) { TVecGetterCmd<T>::invoke(); }
    else if (TclCmd::objc() == itsObjcSet) { TVecSetterCmd<T>::invoke(); }
    else    /* "can't happen" */           { Assert(0); }
  }

private:
  int itsObjcGet;
  int itsObjcSet;
};

///////////////////////////////////////////////////////////////////////
//
// ActionCmd and VecActionCmd class defintions
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

class VecActionCmd : public TclCmd {
public:
  VecActionCmd(TclItemPkg* pkg, const char* cmd_name, Action* action,
               const char* usage, int item_argn) :
    TclCmd(pkg->interp(), cmd_name, 
           usage ? usage : (item_argn ? "item_id(s)" : NULL),
           item_argn+1, item_argn+1),
    itsPkg(pkg),
    itsAction(action),
    itsItemArgn(item_argn) {}

protected:
  virtual void invoke() {
    vector<int> ids;
    if (itsItemArgn) {
      getSequenceFromArg(itsItemArgn, back_inserter(ids), (int*) 0);
    }
    else {
      ids.push_back(-1);
    }

    for (size_t i = 0; i < ids.size(); ++i) {
      void* item = itsPkg->getItemFromId(ids[i]);
      itsAction->action(item);
    }
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
void TclItemPkg::declareGetter_(TclItemPkg* pkg, const char* cmd_name,
                                Getter<T>* getter, const char* usage) {
  pkg->addCommand( new TVecGetterCmd<T>(pkg, pkg->makePkgCmdName(cmd_name), 
                                        getter, usage, pkg->itsItemArgn) );
}

template <class T>
void TclItemPkg::declareSetter_(TclItemPkg* pkg, const char* cmd_name,
                                Setter<T>* setter, const char* usage) {
  pkg->addCommand( new TVecSetterCmd<T>(pkg, pkg->makePkgCmdName(cmd_name),
                                        setter, usage, pkg->itsItemArgn) );
}

template <class T>
void TclItemPkg::declareAttrib_(TclItemPkg* pkg, const char* attrib_name,
                                Attrib<T>* attrib, const char* usage) {
  pkg->addCommand( new TVecAttribCmd<T>(pkg, pkg->makePkgCmdName(attrib_name),
                                        attrib, usage, pkg->itsItemArgn) );
}

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

static const char vcid_tclitempkg_cc[] = "$Header$";
#endif // !TCLITEMPKG_CC_DEFINED
