///////////////////////////////////////////////////////////////////////
//
// tclpkg.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:59 1999
// written: Wed Jul 18 11:25:50 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_H_DEFINED
#define TCLPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJFUNCTOR_H_DEFINED)
#include "tcl/objfunctor.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLPKGBASE_H_DEFINED)
#include "tcl/tclpkgbase.h"
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

  ///////////////////////////////////////////////////////////////////////
  /**
   *
   * Tcl::Pkg extends Tcl::PkgBase by providing a set of functions to
   * define Tcl commands from C++ functors.
   *
   **/
  ///////////////////////////////////////////////////////////////////////

  class Pkg : public PkgBase {
  public:
    Pkg(Tcl_Interp* interp, const char* name, const char* version);

    virtual ~Pkg();

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

    template <class Func>
    inline void defRaw(Func f, const char* cmd_name,
                       const char* usage, unsigned int nargs)
    {
      addCommand( makeGenericCmd(interp(), f, makePkgCmdName(cmd_name),
                                 usage, nargs) );
    }

    template <class Func>
    inline void defVecRaw(Func f, const char* cmd_name,
                          const char* usage, unsigned int nargs,
                          unsigned int keyarg=1)
    {
      addCommand( makeGenericVecCmd(interp(), f, makePkgCmdName(cmd_name),
                                    usage, nargs, keyarg) );
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

    void addIoCommands();

    void addGenericObjCmds(shared_ptr<ObjCaster> caster);

  protected:
    static const char* actionUsage(const char* usage);
    static const char* getterUsage(const char* usage);
    static const char* setterUsage(const char* usage);
  };

  template <class C>
  class CObjCaster : public ObjCaster {
  public:
    virtual bool isMyType(const Util::Object* obj)
    {
      return (obj != 0 && dynamic_cast<const C*>(obj) != 0);
    }
  };

  template <class C>
  void defGenericObjCmds(Pkg* pkg)
  {
    shared_ptr<ObjCaster> caster(new CObjCaster<C>);
    pkg->addGenericObjCmds(caster);
  }

}

static const char vcid_tclpkg_h[] = "$Header$";
#endif // !TCLPKG_H_DEFINED
