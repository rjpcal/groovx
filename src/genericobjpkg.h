///////////////////////////////////////////////////////////////////////
//
// ioitempkg.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul  7 13:17:04 1999
// written: Wed Jun  6 15:54:57 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOITEMPKG_H_DEFINED
#define IOITEMPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IDITEM_H_DEFINED)
#include "util/iditem.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLITEMPKG_H_DEFINED)
#include "tcl/tclitempkg.h"
#endif

namespace Tcl {

/** IoCaster **/

class IoCaster {
protected:
  IoCaster();

public:
  virtual ~IoCaster();

  virtual bool isMyType(const Util::Object* obj) = 0;

  bool isMyType(int id);
};

/** CountAllCmd **/

class CountAllCmd : public TclCmd {
public:
  CountAllCmd(Tcl_Interp* interp, IoCaster* caster, const char* cmd_name);
  virtual ~CountAllCmd();

protected:
  virtual void invoke();

private:
  CountAllCmd(const CountAllCmd&);
  CountAllCmd& operator=(const CountAllCmd&);

  IoCaster* itsCaster;
};

/** FindAllCmd **/

class FindAllCmd : public TclCmd {
public:
  FindAllCmd(Tcl_Interp* interp, IoCaster* caster, const char* cmd_name);
  virtual ~FindAllCmd();

protected:
  virtual void invoke();

private:
  FindAllCmd(const FindAllCmd&);
  FindAllCmd& operator=(const FindAllCmd&);

  IoCaster* itsCaster;
};

/** RemoveAllCmd **/

class RemoveAllCmd : public TclCmd {
public:
  RemoveAllCmd(Tcl_Interp* interp, IoCaster* caster, const char* cmd_name);
  virtual ~RemoveAllCmd();

protected:
  virtual void invoke();

private:
  RemoveAllCmd(const RemoveAllCmd&);
  RemoveAllCmd& operator=(const RemoveAllCmd&);

  IoCaster* itsCaster;
};

/** IsCmd **/

class IsCmd : public TclCmd {
public:
  IsCmd(Tcl_Interp* interp, IoCaster* caster, const char* cmd_name);
  virtual ~IsCmd();

protected:
  virtual void invoke();

private:
  IsCmd(const IsCmd&);
  IsCmd& operator=(const IsCmd&);

  IoCaster* itsCaster;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * IoItemPkg
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class IoItemPkg : public CTclItemPkg<C>, public IoCaster {
public:
  IoItemPkg(Tcl_Interp* interp, const char* name, const char* version) :
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
		IdItem<C> item(id);
		return item.get();
	 }

  virtual bool isMyType(const Util::Object* obj)
    { return (obj != 0 && dynamic_cast<const C*>(obj) != 0); }
};

} // end namespace Tcl

static const char vcid_ioitempkg_h[] = "$Header$";
#endif // !IOITEMPKG_H_DEFINED
