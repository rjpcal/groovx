///////////////////////////////////////////////////////////////////////
//
// tclitempkg.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:59 1999
// written: Mon Jul 16 10:35:05 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_H_DEFINED
#define TCLITEMPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJFUNCTOR_H_DEFINED)
#include "tcl/objfunctor.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLPKG_H_DEFINED)
#include "tcl/tclpkg.h"
#endif

namespace Util { class Object; }

namespace Tcl
{

  /** ObjCaster **/

  class ObjCaster {
  protected:
    ObjCaster();

  public:
    virtual ~ObjCaster();

    virtual bool isMyType(const Util::Object* obj) = 0;

    bool isNotMyType(const Util::Object* obj) { return !isMyType(obj); }

    bool isIdMyType(Util::UID uid);

    bool isIdNotMyType(Util::UID uid) { return !isIdMyType(uid); }
  };

  template <class C>
  class CObjCaster : public ObjCaster {
  public:
    virtual bool isMyType(const Util::Object* obj)
    {
      return (obj != 0 && dynamic_cast<const C*>(obj) != 0);
    }
  };

  void doAddGenericObjCmds(TclPkg* pkg, shared_ptr<ObjCaster> caster);

  template <class C>
  void defGenericObjCmds(TclPkg* pkg)
  {
    shared_ptr<ObjCaster> caster(new CObjCaster<C>);
    doAddGenericObjCmds(pkg, caster);
  }

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

  template <class Func>
  inline void def(Func f, const char* cmd_name, const char* usage)
  {
    addCommand( makeCmd(interp(), f, makePkgCmdName(cmd_name), usage) );
  }

  template <class Func>
  inline void defVec(Func f, const char* cmd_name,
                     const char* usage, unsigned int keyarg=1)
  {
    addCommand( makeVecCmd(interp(), f, makePkgCmdName(cmd_name),
                           usage, keyarg) );
  }

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) (),
                 const char* usage = 0)
    {
      defVec( actionFunc, cmd_name, actionUsage(usage) );
    }

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) () const,
                 const char* usage = 0)
    {
      defVec( actionFunc, cmd_name, actionUsage(usage) );
    }

  template <class C, class T>
  void defGetter(const char* cmd_name, T (C::* getterFunc) () const,
                 const char* usage = 0)
    {
      defVec( getterFunc, cmd_name, getterUsage(usage) );
    }

  template <class C, class T>
  void defSetter(const char* cmd_name, void (C::* setterFunc) (T),
                 const char* usage = 0)
    {
      defVec( setterFunc, cmd_name, setterUsage(usage) );
    }

  template <class C, class T>
  void defAttrib(const char* cmd_name,
                 T (C::* getterFunc) () const,
                 void (C::* setterFunc) (T),
                 const char* usage = 0)
    {
      defGetter( cmd_name, getterFunc, usage );
      defSetter( cmd_name, setterFunc, usage );
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
