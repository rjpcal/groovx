///////////////////////////////////////////////////////////////////////
//
// genericobjpkg.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul  7 13:17:04 1999
// written: Thu Jul 12 13:23:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GENERICOBJPKG_H_DEFINED
#define GENERICOBJPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REF_H_DEFINED)
#include "util/ref.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLITEMPKG_H_DEFINED)
#include "tcl/tclitempkg.h"
#endif

namespace Tcl {

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

/** CountAllCmd **/

class CountAllCmd : public TclCmd {
public:
  CountAllCmd(Tcl_Interp* interp, ObjCaster* caster, const char* cmd_name);
  virtual ~CountAllCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);

private:
  CountAllCmd(const CountAllCmd&);
  CountAllCmd& operator=(const CountAllCmd&);

  ObjCaster* itsCaster;
};

/** FindAllCmd **/

class FindAllCmd : public TclCmd {
public:
  FindAllCmd(Tcl_Interp* interp, ObjCaster* caster, const char* cmd_name);
  virtual ~FindAllCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);

private:
  FindAllCmd(const FindAllCmd&);
  FindAllCmd& operator=(const FindAllCmd&);

  ObjCaster* itsCaster;
};

/** RemoveAllCmd **/

class RemoveAllCmd : public TclCmd {
public:
  RemoveAllCmd(Tcl_Interp* interp, ObjCaster* caster, const char* cmd_name);
  virtual ~RemoveAllCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);

private:
  RemoveAllCmd(const RemoveAllCmd&);
  RemoveAllCmd& operator=(const RemoveAllCmd&);

  ObjCaster* itsCaster;
};

/** IsCmd **/

class IsCmd : public TclCmd {
public:
  IsCmd(Tcl_Interp* interp, ObjCaster* caster, const char* cmd_name);
  virtual ~IsCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);

private:
  IsCmd(const IsCmd&);
  IsCmd& operator=(const IsCmd&);

  ObjCaster* itsCaster;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * GenericObjPkg
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class GenericObjPkg : public CTclItemPkg<C>, public ObjCaster {
public:
  GenericObjPkg(Tcl_Interp* interp, const char* name, const char* version) :
    CTclItemPkg<C>(interp, name, version, 1)
  {
    addCommand( new IsCmd(interp, this, TclPkg::makePkgCmdName("is")));
    addCommand( new CountAllCmd(interp, this,
                                TclPkg::makePkgCmdName("countAll")));
    addCommand( new FindAllCmd(interp, this,
                               TclPkg::makePkgCmdName("findAll")));
    addCommand( new RemoveAllCmd(interp, this,
                                 TclPkg::makePkgCmdName("removeAll")));
  }

  virtual C* getCItemFromId(int id)
    {
      Util::WeakRef<C> item(id);
      return item.get();
    }

  virtual bool isMyType(const Util::Object* obj)
    { return (obj != 0 && dynamic_cast<const C*>(obj) != 0); }
};

} // end namespace Tcl

static const char vcid_genericobjpkg_h[] = "$Header$";
#endif // !GENERICOBJPKG_H_DEFINED
