///////////////////////////////////////////////////////////////////////
//
// tclveccmds.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 12:11:41 1999
// written: Thu Jul 12 16:31:21 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMDS_H_DEFINED
#define TCLVECCMDS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
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

class ItemFetcher;

///////////////////////////////////////////////////////////////////////
/**
 *
 * TVecGetterCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class ValType>
class TVecGetterCmd : public Tcl::VecCmd {
public:
  TVecGetterCmd(Tcl_Interp* interp, ItemFetcher* fetcher, const char* cmd_name,
                shared_ptr<Getter<ValType> > getter,
                const char* usage, unsigned int item_argn);

  virtual ~TVecGetterCmd();

protected:
  virtual void invoke(Context& ctx);

private:
  TVecGetterCmd(const TVecGetterCmd&);
  TVecGetterCmd& operator=(const TVecGetterCmd&);

  ItemFetcher* itsFetcher;
  int itsItemArgn;
  shared_ptr< Getter<ValType> > itsGetter;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * TVecSetterCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class TVecSetterCmd : public Tcl::VecCmd {
public:
  typedef typename SetterCmdTraits<T>::value_type value_type;
  typedef typename SetterCmdTraits<T>::stack_type stack_type;
  typedef typename SetterCmdTraits<T>::iterator_type iterator_type;

  TVecSetterCmd(Tcl_Interp* interp, ItemFetcher* fetcher, const char* cmd_name,
                 shared_ptr<Setter<value_type> > setter,
                 const char* usage, unsigned int item_argn);

  virtual ~TVecSetterCmd();

  virtual void invoke(Context& ctx);

private:
  TVecSetterCmd(const TVecSetterCmd&);
  TVecSetterCmd& operator=(const TVecSetterCmd&);

  ItemFetcher* itsFetcher;
  int itsItemArgn;
  int itsValArgn;
  shared_ptr< Setter<value_type> > itsSetter;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * TVecAttribCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class TVecAttribCmd : public TVecGetterCmd<T> {
public:
  TVecAttribCmd(Tcl_Interp* interp, ItemFetcher* fetcher, const char* cmd_name,
                shared_ptr<Getter<T> > getter,
                shared_ptr<Setter<T> > setter,
                const char* usage, unsigned int item_argn);

  virtual ~TVecAttribCmd();
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
  VecActionCmd(Tcl_Interp* interp, ItemFetcher* fetcher, const char* cmd_name,
               shared_ptr<Action> action,
               const char* usage, unsigned int item_argn);
  virtual ~VecActionCmd();

protected:
  virtual void invoke(Context& ctx);

private:
  VecActionCmd(const VecActionCmd&);
  VecActionCmd& operator=(const VecActionCmd&);

  ItemFetcher* itsFetcher;
  shared_ptr<Action> itsAction;
  int itsItemArgn;
};

static const char vcid_tclveccmds_h[] = "$Header$";
#endif // !TCLVECCMDS_H_DEFINED
