///////////////////////////////////////////////////////////////////////
//
// tclveccmds.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 12:11:41 1999
// written: Thu Jul 12 14:00:51 2001
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

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLVECCMD_H_DEFINED)
#include "tcl/tclveccmd.h"
#endif

template <class T> class Getter;
template <class T> class Setter;

class Action;

class fixed_string;


namespace Tcl
{
  class VecActionCmd;

  template <class T>
  struct SetterCmdTraits {
    typedef T value_type;
    typedef T stack_type;
    typedef List::Iterator<T> iterator_type;
  };

  template <>
  struct SetterCmdTraits<const fixed_string&> {
    typedef const fixed_string& value_type;
    typedef const char* stack_type;
    typedef List::Iterator<const char*> iterator_type;
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

class VecGetterBaseCmd : public virtual Tcl::VecCmd {
public:
  VecGetterBaseCmd(TclItemPkgBase* pkg, const char* cmd_name,
                   const char* usage, int item_argn);

  virtual ~VecGetterBaseCmd();

protected:
  virtual void invoke(Context& ctx);

  virtual void doReturnValForItem(void* item, Context& ctx) = 0;
  virtual void doAppendValForItem(void* item, Tcl::List& listObj) = 0;

private:
  VecGetterBaseCmd(const VecGetterBaseCmd&);
  VecGetterBaseCmd& operator=(const VecGetterBaseCmd&);

  TclItemPkgBase* itsPkg;
  int itsItemArgn;
};

template <class ValType>
class TVecGetterCmd : public VecGetterBaseCmd {
public:
  TVecGetterCmd(TclItemPkgBase* pkg, const char* cmd_name,
                shared_ptr<Getter<ValType> > getter,
                const char* usage, int item_argn);

  virtual ~TVecGetterCmd();

protected:
  virtual void doReturnValForItem(void* item, Context& ctx);
  virtual void doAppendValForItem(void* item, Tcl::List& listObj);

private:
  shared_ptr< Getter<ValType> > itsGetter;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * TVecSetterCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

class VecSetterBaseCmd : public virtual Tcl::VecCmd {
public:
  VecSetterBaseCmd(TclItemPkgBase* pkg, const char* cmd_name,
                   const char* usage, int item_argn);

  virtual ~VecSetterBaseCmd();

protected:
  virtual void invoke(Context& ctx);

  virtual void invokeForItemArgn(Context& ctx, int item_argn, int val_argn) = 0;
  virtual void setSingleItem(Context& ctx, void* item, int val_argn) = 0;

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
  typedef typename Traits::value_type value_type;
  typedef typename Traits::stack_type stack_type;
  typedef typename Traits::iterator_type iterator_type;

  TrVecSetterCmd(TclItemPkgBase* pkg, const char* cmd_name,
                 shared_ptr<Setter<value_type> > setter,
                 const char* usage, int item_argn);

  virtual ~TrVecSetterCmd();

  virtual void invokeForItemArgn(Context& ctx, int item_argn, int val_argn);
  virtual void setSingleItem(Context& ctx, void* item, int val_argn);

private:
  shared_ptr< Setter<value_type> > itsSetter;
};

template <class T>
class TVecSetterCmd : public TrVecSetterCmd< SetterCmdTraits<T> > {
public:
  typedef TrVecSetterCmd< SetterCmdTraits<T> > Base;

  TVecSetterCmd(TclItemPkgBase* pkg, const char* cmd_name,
                shared_ptr<Setter<T> > setter,
                const char* usage, int item_argn);

  virtual ~TVecSetterCmd();
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
  TVecAttribCmd(TclItemPkgBase* pkg, const char* cmd_name,
                shared_ptr<Getter<T> > getter,
                shared_ptr<Setter<T> > setter,
                const char* usage, int item_argn);

  virtual ~TVecAttribCmd();

protected:
  virtual void invoke(Context& ctx);

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

class Tcl::VecActionCmd : public Tcl::VecCmd {
public:
  VecActionCmd(TclItemPkgBase* pkg, const char* cmd_name,
               shared_ptr<Action> action,
               const char* usage, int item_argn);
  virtual ~VecActionCmd();

protected:
  virtual void invoke(Context& ctx);

private:
  VecActionCmd(const VecActionCmd&);
  VecActionCmd& operator=(const VecActionCmd&);

  TclItemPkgBase* itsPkg;
  shared_ptr<Action> itsAction;
  int itsItemArgn;
};

static const char vcid_tclveccmds_h[] = "$Header$";
#endif // !TCLVECCMDS_H_DEFINED
