///////////////////////////////////////////////////////////////////////
//
// tclpkg.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:59 1999
// written: Sun Aug 26 08:35:10 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_H_DEFINED
#define TCLPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLMEMFUNCTOR_H_DEFINED)
#include "tcl/tclmemfunctor.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLPKGBASE_H_DEFINED)
#include "tcl/tclpkgbase.h"
#endif

namespace Tcl
{
  class ObjCaster;
  class Pkg;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::Pkg extends Tcl::PkgBase by providing a set of functions to
 * define Tcl commands from C++ functors.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Pkg : public PkgBase
{
public:
  Pkg(Tcl_Interp* interp, const char* pkg_name, const char* version);

  virtual ~Pkg();

  template <class Func>
  inline void def(const char* cmd_name, const char* usage, Func f)
  {
    addCommand( makeCmd(interp(), f, makePkgCmdName(cmd_name), usage) );
  }

  template <class Func>
  inline void defVec(const char* cmd_name, const char* usage, Func f,
                     unsigned int keyarg=1)
  {
    addCommand( makeVecCmd(interp(), f, makePkgCmdName(cmd_name),
                           usage, keyarg) );
  }

  template <class Func>
  inline void defRaw(const char* cmd_name, unsigned int nargs,
                     const char* usage, Func f)
  {
    addCommand( makeGenericCmd(interp(), f, makePkgCmdName(cmd_name),
                               usage, nargs) );
  }

  template <class Func>
  inline void defVecRaw(const char* cmd_name, unsigned int nargs,
                        const char* usage, Func f,
                        unsigned int keyarg=1)
  {
    addCommand( makeGenericVecCmd(interp(), f, makePkgCmdName(cmd_name),
                                  usage, nargs, keyarg) );
  }

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) ())
  {
    defVec( cmd_name, actionUsage(""), actionFunc );
  }

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) () const)
  {
    defVec( cmd_name, actionUsage(""), actionFunc );
  }

  template <class C, class T>
  void defGetter(const char* cmd_name, const char* usage,
                 T (C::* getterFunc) () const)
  {
    defVec( cmd_name, getterUsage(usage), getterFunc );
  }

  template <class C, class T>
  void defGetter(const char* cmd_name, T (C::* getterFunc) () const)
  {
    defVec( cmd_name, getterUsage(""), getterFunc );
  }

  template <class C, class T>
  void defSetter(const char* cmd_name, const char* usage,
                 void (C::* setterFunc) (T))
  {
    defVec( cmd_name, setterUsage(usage), setterFunc );
  }

  template <class C, class T>
  void defSetter(const char* cmd_name, void (C::* setterFunc) (T))
  {
    defVec( cmd_name, setterUsage(""), setterFunc );
  }

  template <class C, class T>
  void defAttrib(const char* cmd_name,
                 T (C::* getterFunc) () const,
                 void (C::* setterFunc) (T))
  {
    defGetter( cmd_name, getterFunc );
    defSetter( cmd_name, setterFunc );
  }

  void defIoCommands();

  void defGenericObjCmds(shared_ptr<ObjCaster> caster);

protected:
  static const char* actionUsage(const char* usage);
  static const char* getterUsage(const char* usage);
  static const char* setterUsage(const char* usage);
};


namespace Util { class Object; }

/** ObjCaster **/

class Tcl::ObjCaster
{
protected:
  ObjCaster();

public:
  virtual ~ObjCaster();

  virtual bool isMyType(const Util::Object* obj) = 0;

  bool isNotMyType(const Util::Object* obj) { return !isMyType(obj); }

  bool isIdMyType(Util::UID uid);

  bool isIdNotMyType(Util::UID uid) { return !isIdMyType(uid); }
};

namespace Tcl
{
  template <class C>
  class CObjCaster : public ObjCaster
  {
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
    pkg->defGenericObjCmds(caster);
  }
}

static const char vcid_tclpkg_h[] = "$Header$";
#endif // !TCLPKG_H_DEFINED
