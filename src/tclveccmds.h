///////////////////////////////////////////////////////////////////////
//
// tclveccmds.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 12:11:41 1999
// written: Wed May 17 14:09:23 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMDS_H_DEFINED
#define TCLVECCMDS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCMD_H_DEFINED)
#include "tcl/tclcmd.h"
#endif

template <class T> class Getter;
template <class T> class Setter;
template <class T> class Attrib;

class Action;

class fixed_string;


namespace Tcl {
  class VecActionCmd;

  template <class T>
	 class SetterCmdTraits {
		typedef T value_type;
		typedef T stack_type;
		typedef ListIterator<T> iterator_type;
	 };

  template <>
	 class SetterCmdTraits<const fixed_string&> {
		typedef const fixed_string& value_type;
		typedef const char* stack_type;
		typedef ListIterator<const char*> iterator_type;
	 };
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

protected:
  virtual void invoke();

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

protected:
  virtual void invoke();

  virtual void invokeForItemArgn(int item_argn, int val_argn) = 0;
  virtual void setSingleItem(void* item, int val_argn) = 0;

  TclItemPkgBase* pkg() { return itsPkg; }

private:
  VecSetterBaseCmd(const VecSetterBaseCmd&);
  VecSetterBaseCmd& operator=(const VecSetterBaseCmd&);

  TclItemPkgBase* itsPkg;
  int itsItemArgn;
  int itsValArgn;
};

template <class Traits>
class TrVecSetterCmd : public VecSetterBaseCmd {
protected:
  typedef typename Traits::value_type T;
  typedef typename Traits::stack_type stack_type;
  typedef typename Traits::iterator_type iterator_type;

  TrVecSetterCmd(TclItemPkgBase* pkg, const char* cmd_name, Setter<T>* setter,
                const char* usage, int item_argn);

  virtual void invokeForItemArgn(int item_argn, int val_argn);
  virtual void setSingleItem(void* item, int val_argn);

private:
  scoped_ptr< Setter<T> > itsSetter;
};

template <class T>
class TVecSetterCmd : public TrVecSetterCmd< SetterCmdTraits<T> > {
public:
  typedef TVecSetterCmd::T Type;
  typedef TrVecSetterCmd< SetterCmdTraits<Type> > Base;
  TVecSetterCmd(TclItemPkgBase* pkg, const char* cmd_name,
					 Setter<Type>* setter, const char* usage, int item_argn);
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
  typedef TVecAttribCmd::T Type;
  TVecAttribCmd(TclItemPkgBase* pkg, const char* cmd_name,
					 Attrib<Type>* attrib, const char* usage, int item_argn);

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
