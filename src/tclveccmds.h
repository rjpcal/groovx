///////////////////////////////////////////////////////////////////////
//
// tclveccmds.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 12:11:41 1999
// written: Wed Mar 15 10:54:11 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMDS_H_DEFINED
#define TCLVECCMDS_H_DEFINED

#ifndef POINTERS_H_DEFINED
#include "util/pointers.h"
#endif

#ifndef TCLCMD_H_DEFINED
#include "tcl/tclcmd.h"
#endif

template <class T> class Getter;
template <class T> class Setter;
template <class T> class Attrib;

class Action;


namespace Tcl {
  class VecActionCmd;
}

namespace Tcl {

class TclItemPkgBase;

///////////////////////////////////////////////////////////////////////
/**
 *
 * TVecGetterCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

class VecGetterBaseCmd : public virtual TclCmd {
public:
  VecGetterBaseCmd(TclItemPkgBase* pkg, const char* cmd_name,
						 const char* usage, int item_argn);

  virtual void invoke();

protected:
  virtual void doReturnValForItem(void* item) = 0;
  virtual void doAppendValForItem(void* item) = 0;

private:
  VecGetterBaseCmd(const VecGetterBaseCmd&);
  VecGetterBaseCmd& operator=(const VecGetterBaseCmd&);

  TclItemPkgBase* itsPkg;
  int itsItemArgn;
};

template <class ValType>
class TVecGetterCmd : public VecGetterBaseCmd {
public:
  TVecGetterCmd(TclItemPkgBase* pkg, const char* cmd_name, Getter<ValType>* getter,
                const char* usage, int item_argn);

protected:
  virtual void doReturnValForItem(void* item);
  virtual void doAppendValForItem(void* item);

private:
  scoped_ptr< Getter<ValType> > itsGetter;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * TVecSetterCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

class VecSetterBaseCmd : public virtual TclCmd {
public:
  VecSetterBaseCmd(TclItemPkgBase* pkg, const char* cmd_name,
						 const char* usage, int item_argn);

  virtual void invoke();

protected:
  virtual void* getValVec(int val_argn, int num_ids, unsigned int& num_vals) = 0;
  virtual void setValForItem(void* item, void* val_vec, unsigned int valn) = 0;
  virtual void destroyValVec(void* val_vec) = 0;

private:
  VecSetterBaseCmd(const VecSetterBaseCmd&);
  VecSetterBaseCmd& operator=(const VecSetterBaseCmd&);

  TclItemPkgBase* itsPkg;
  int itsItemArgn;
  int itsValArgn;
};

template <class T>
class TVecSetterCmd : public VecSetterBaseCmd {
public:
  TVecSetterCmd(TclItemPkgBase* pkg, const char* cmd_name, Setter<T>* setter,
                const char* usage, int item_argn);

protected:
  virtual void* getValVec(int val_argn, int num_ids, unsigned int& num_vals);
  virtual void setValForItem(void* item, void* val_vec, unsigned int valn);
  virtual void destroyValVec(void* val_vec);

private:
  scoped_ptr< Setter<T> > itsSetter;
};

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
  TVecAttribCmd(TclItemPkgBase* pkg, const char* cmd_name, Attrib<T>* attrib,
                const char* usage, int item_argn);

protected:
  virtual void invoke();

private:
  int itsObjcGet;
  int itsObjcSet;
};

} // end namespace Tcl

///////////////////////////////////////////////////////////////////////
/**
 *
 * VecActionCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::VecActionCmd : public Tcl::TclCmd {
public:
  VecActionCmd(TclItemPkgBase* pkg, const char* cmd_name, Action* action,
               const char* usage, int item_argn);

protected:
  virtual void invoke();

private:
  VecActionCmd(const VecActionCmd&);
  VecActionCmd& operator=(const VecActionCmd&);

  TclItemPkgBase* itsPkg;
  scoped_ptr<Action> itsAction;
  int itsItemArgn;
};

static const char vcid_tclveccmds_h[] = "$Header$";
#endif // !TCLVECCMDS_H_DEFINED
