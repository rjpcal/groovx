///////////////////////////////////////////////////////////////////////
//
// tclveccmds.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 12:11:41 1999
// written: Thu Dec 16 11:52:16 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMDS_H_DEFINED
#define TCLVECCMDS_H_DEFINED

#ifndef MEMORY_DEFINED
#include <memory>
#define MEMORY_DEFINED
#endif

#ifndef TCLCMD_H_DEFINED
#include "tclcmd.h"
#endif

template <class T> class Getter;
template <class T> class Setter;
template <class T> class Attrib;

class Action;


namespace Tcl {

class TclItemPkg;

///////////////////////////////////////////////////////////////////////
/**
 *
 * TVecGetterCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

class VecGetterBaseCmd : public virtual TclCmd {
public:
  VecGetterBaseCmd(TclItemPkg* pkg, const char* cmd_name,
						 const char* usage, int item_argn);

  virtual void invoke();

protected:
  virtual void doReturnValForItem(void* item) = 0;
  virtual void doAppendValForItem(void* item) = 0;

private:
  TclItemPkg* itsPkg;
  int itsItemArgn;
};

template <class ValType>
class TVecGetterCmd : public VecGetterBaseCmd {
public:
  TVecGetterCmd(TclItemPkg* pkg, const char* cmd_name, Getter<ValType>* getter,
                const char* usage, int item_argn);

protected:
  virtual void doReturnValForItem(void* item);
  virtual void doAppendValForItem(void* item);

private:
  auto_ptr< Getter<ValType> > itsGetter;
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
  VecSetterBaseCmd(TclItemPkg* pkg, const char* cmd_name,
						 const char* usage, int item_argn);

  virtual void invoke();

protected:
  virtual void* getValVec(int val_argn, size_t& num_vals) = 0;
  virtual void setValForItem(void* item, void* val_vec, size_t valn) = 0;
  virtual void destroyValVec(void* val_vec) = 0;

private:
  TclItemPkg* itsPkg;
  int itsItemArgn;
  int itsValArgn;
};

template <class T>
class TVecSetterCmd : public VecSetterBaseCmd {
public:
  TVecSetterCmd(TclItemPkg* pkg, const char* cmd_name, Setter<T>* setter,
                const char* usage, int item_argn);

protected:
  virtual void* getValVec(int val_argn, size_t& num_vals);
  virtual void setValForItem(void* item, void* val_vec, size_t valn);
  virtual void destroyValVec(void* val_vec);

private:
  auto_ptr< Setter<T> > itsSetter;
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
  TVecAttribCmd(TclItemPkg* pkg, const char* cmd_name, Attrib<T>* attrib,
                const char* usage, int item_argn);

protected:
  virtual void invoke();

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
               const char* usage, int item_argn);

protected:
  virtual void invoke();

private:
  TclItemPkg* itsPkg;
  auto_ptr<Action> itsAction;
  int itsItemArgn;

};

} // end namespace Tcl

static const char vcid_tclveccmds_h[] = "$Header$";
#endif // !TCLVECCMDS_H_DEFINED
