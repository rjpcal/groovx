///////////////////////////////////////////////////////////////////////
//
// tclitempkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:54 1999
// written: Mon Jul 16 07:12:17 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_CC_DEFINED
#define TCLITEMPKG_CC_DEFINED

#ifdef ACC_COMPILER // aCC needs this to be happy with the header
#include "util/ref.h"
#endif

#include "tcl/tclitempkg.h"

#include "tcl/stringifycmd.h"
#include "tcl/tclcmd.h"
#include "tcl/tclveccmds.h"

#include "util/strings.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_DEBUG
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TclItemPkgBase member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TclItemPkgBase::TclItemPkgBase(Tcl_Interp* interp,
                                    const char* name, const char* version) :
  TclPkg(interp, name, version)
{}

Tcl::TclItemPkgBase::~TclItemPkgBase() {}

void* Tcl::TclItemPkgBase::getItemFromContext(Tcl::Context& ctx)
{
  int id = ctx.getIntFromArg(1);
  return getItemFromId(id);
}

///////////////////////////////////////////////////////////////////////
//
// TclItemPkg member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TclItemPkg::TclItemPkg(Tcl_Interp* interp,
                            const char* name, const char* version) :
  TclItemPkgBase(interp, name, version)
{}

Tcl::TclItemPkg::~TclItemPkg() {}

const char* Tcl::TclItemPkg::actionUsage(const char* usage)
{
  return usage ? usage : "item_id(s)";
}

// NOTE: aCC won't compile these TclItemPkg functions if they are
// defined inside a namespace Tcl {} block. So, as a workaround, we
// must define them at global scope, using explicit Tcl:: qualifiers

template <class T>
void Tcl::TclItemPkg::declareGetter(const char* cmd_name,
                                    shared_ptr<Getter<T> > getter,
                                    const char* usage)
{
  addCommand(new Tcl::TVecGetterCmd<T>(interp(), this,
                                       makePkgCmdName(cmd_name),
                                       getter,
                                       usage));
}

template <class T>
void Tcl::TclItemPkg::declareSetter(const char* cmd_name,
                                    shared_ptr<Setter<T> > setter,
                                    const char* usage)
{
  addCommand(new Tcl::TVecSetterCmd<T>(interp(), this,
                                       makePkgCmdName(cmd_name),
                                       setter,
                                       usage));
}

template <class T>
void Tcl::TclItemPkg::declareAttrib(const char* attrib_name,
                                    shared_ptr<Getter<T> > getter,
                                    shared_ptr<Setter<T> > setter,
                                    const char* usage)
{
  declareGetter<T>(attrib_name, getter, usage);
  declareSetter<T>(attrib_name, setter, usage);
}

void Tcl::TclItemPkg::instantiate()
{
#define DUMMY_INST(type) \
  declareGetter(0,shared_ptr<Getter<type> >(0),0); \
  declareSetter(0,shared_ptr<Setter<type> >(0),0); \
  declareAttrib(0,shared_ptr<Getter<type> >(0),shared_ptr<Setter<type> >(0),0);

  DUMMY_INST(int);
  DUMMY_INST(unsigned int);
  DUMMY_INST(unsigned long);
  DUMMY_INST(bool);
  DUMMY_INST(double);
  DUMMY_INST(const char*);
  DUMMY_INST(fixed_string);
  DUMMY_INST(const fixed_string&);
#undef DUMMY_INST
}

void Tcl::TclItemPkg::declareAction(const char* action_name,
                                    shared_ptr<Action> action,
                                    const char* usage)
{
  addCommand( new VecActionCmd(interp(), this, makePkgCmdName(action_name),
                               action, usage) );
}


///////////////////////////////////////////////////////////////////////
//
// IO command definitions
//
///////////////////////////////////////////////////////////////////////

void Tcl::TclItemPkg::addIoCommands()
{
DOTRACE("Tcl::TclItemPkg::addIoCommands");
  addCommand( new StringifyCmd(interp(), makePkgCmdName("stringify")) );
  addCommand( new DestringifyCmd(interp(), makePkgCmdName("destringify")) );

  addCommand( new WriteCmd(interp(), makePkgCmdName("write")) );
  addCommand( new ReadCmd(interp(), makePkgCmdName("read")) );

  addCommand( new ASWSaveCmd(interp(), makePkgCmdName("save")) );
  addCommand( new ASRLoadCmd(interp(), makePkgCmdName("load")) );
}

static const char vcid_tclitempkg_cc[] = "$Header$";
#endif // !TCLITEMPKG_CC_DEFINED
