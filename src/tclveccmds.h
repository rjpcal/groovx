///////////////////////////////////////////////////////////////////////
//
// tclveccmds.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 12:11:41 1999
// written: Tue Dec  7 12:15:38 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMDS_H_DEFINED
#define TCLVECCMDS_H_DEFINED

#ifndef TCLCMD_H_DEFINED
#include "tclcmd.h"
#endif

class TclItemPkg;

///////////////////////////////////////////////////////////////////////
/**
 *
 * TVecGetterCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

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
/**
 *
 * TVecSetterCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

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

    size_t max_valn = vals.size()-1;

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

    size_t max_valn = vals.size()-1;

    for (size_t i = 0, valn = 0; i < ids.size(); ++i) {
      void* item = itsPkg->getItemFromId(ids[i]);
      itsSetter->set(item, vals[valn]);
      if (valn < max_valn) ++valn;
    }

    returnVoid();
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * TVecAttribCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

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
/**
 *
 * VecActionCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

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

static const char vcid_tclveccmds_h[] = "$Header$";
#endif // !TCLVECCMDS_H_DEFINED
