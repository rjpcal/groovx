///////////////////////////////////////////////////////////////////////
//
// tclitempkg.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:59 1999
// written: Mon Jul 16 10:09:10 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_H_DEFINED
#define TCLITEMPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJFUNCTOR_H_DEFINED)
#include "tcl/objfunctor.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLPKG_H_DEFINED)
#include "tcl/tclpkg.h"
#endif

namespace Tcl
{

///////////////////////////////////////////////////////////////////////
/**
 *
 * TclItemPkg class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

class TclItemPkg : public TclPkg {
public:
  TclItemPkg(Tcl_Interp* interp, const char* name, const char* version);

  virtual ~TclItemPkg();

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) (),
                 const char* usage = 0)
    {
      Tcl::defVec( this, actionFunc,
                   makePkgCmdName(cmd_name), actionUsage(usage) );
    }

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) () const,
                 const char* usage = 0)
    {
      Tcl::defVec( this, actionFunc,
                   makePkgCmdName(cmd_name), actionUsage(usage) );
    }

  template <class C, class T>
  void defGetter(const char* cmd_name, T (C::* getterFunc) () const,
                 const char* usage = 0)
    {
      Tcl::defVec( this, getterFunc,
                   makePkgCmdName(cmd_name), getterUsage(usage) );
    }

  template <class C, class T>
  void defSetter(const char* cmd_name, void (C::* setterFunc) (T),
                 const char* usage = 0)
    {
      Tcl::defVec( this, setterFunc,
                   makePkgCmdName(cmd_name), setterUsage(usage) );
    }

  template <class C, class T>
  void defAttrib(const char* cmd_name,
                 T (C::* getterFunc) () const,
                 void (C::* setterFunc) (T),
                 const char* usage = 0)
    {
      Tcl::defVec( this, getterFunc,
                   makePkgCmdName(cmd_name), getterUsage(usage) );
      Tcl::defVec( this, setterFunc,
                   makePkgCmdName(cmd_name), setterUsage(usage) );
    }

protected:

  void addIoCommands();

  static const char* actionUsage(const char* usage);
  static const char* getterUsage(const char* usage);
  static const char* setterUsage(const char* usage);

private:
  void instantiate();
};

}

static const char vcid_tclitempkg_h[] = "$Header$";
#endif // !TCLITEMPKG_H_DEFINED
