///////////////////////////////////////////////////////////////////////
//
// tclveccmds.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 12:11:41 1999
// written: Tue Dec  7 19:17:56 1999
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

template <class ValType>
class TVecGetterCmd : public virtual TclCmd {
public:
  TVecGetterCmd(TclItemPkg* pkg, const char* cmd_name, Getter<ValType>* getter,
                const char* usage, int item_argn);

protected:
  virtual void invoke();

private:
  TclItemPkg* itsPkg;
  auto_ptr< Getter<ValType> > itsGetter;
  int itsItemArgn;
};

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
                const char* usage, int item_argn);

protected:
  virtual void invoke();

private:
  TclItemPkg* itsPkg;
  auto_ptr< Setter<T> > itsSetter;
  int itsItemArgn;
  int itsValArgn;
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

  ~TVecAttribCmd();

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
