///////////////////////////////////////////////////////////////////////
//
// tclitempkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:54 1999
// written: Thu Jul 12 13:13:24 2001
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
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TclItemPkg member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TclItemPkg::TclItemPkg(Tcl_Interp* interp,
                            const char* name, const char* version,
                            int item_argn) :
  TclItemPkgBase(interp, name, version, item_argn)
{}

Tcl::TclItemPkg::~TclItemPkg() {}

// NOTE: aCC won't compile these TclItemPkg functions if they are
// defined inside a namespace Tcl {} block. So, as a workaround, we
// must define them at global scope, using explicit Tcl:: qualifiers

template <class T>
void Tcl::TclItemPkg::declareGetter(const char* cmd_name,
                                    Getter<T>* getter, const char* usage)
{
  addCommand(
         new Tcl::TVecGetterCmd<T>(interp(), this, makePkgCmdName(cmd_name),
#ifndef ACC_COMPILER
                                   make_shared(getter),
#else
                                   shared_ptr<Getter<T> >(getter),
#endif
                                   usage, itemArgn())
         );
}

template <class T>
void Tcl::TclItemPkg::declareSetter(const char* cmd_name,
                                    Setter<T>* setter, const char* usage)
{
  addCommand(
         new Tcl::TVecSetterCmd<T>(interp(), this, makePkgCmdName(cmd_name),
#ifndef ACC_COMPILER
                                   make_shared(setter),
#else
                                   shared_ptr<Setter<T> >(setter),
#endif
                                   usage, itemArgn())
         );
}

template <class T>
void Tcl::TclItemPkg::declareAttrib(const char* attrib_name,
                                    Getter<T>* getter,
                                    Setter<T>* setter,
                                    const char* usage)
{
  addCommand(
         new Tcl::TVecAttribCmd<T>(interp(), this, makePkgCmdName(attrib_name),
#ifndef ACC_COMPILER
                                   make_shared(getter),
                                   make_shared(setter),
#else
                                   shared_ptr<Getter<T> >(getter),
                                   shared_ptr<Setter<T> >(setter),
#endif
                                   usage, itemArgn())
         );
}

void Tcl::TclItemPkg::instantiate()
{
  declareGetter(0, (Getter<int>*) 0, 0);
  declareGetter(0, (Getter<unsigned int>*) 0, 0);
  declareGetter(0, (Getter<unsigned long>*) 0, 0);
  declareGetter(0, (Getter<bool>*) 0, 0);
  declareGetter(0, (Getter<double>*) 0, 0);
  declareGetter(0, (Getter<const char*>*) 0, 0);
  declareGetter(0, (Getter<fixed_string>*) 0, 0);
  declareGetter(0, (Getter<const fixed_string&>*) 0, 0);

  declareSetter(0, (Setter<int>*) 0, 0);
  declareSetter(0, (Setter<unsigned int>*) 0, 0);
  declareSetter(0, (Setter<unsigned long>*) 0, 0);
  declareSetter(0, (Setter<bool>*) 0, 0);
  declareSetter(0, (Setter<double>*) 0, 0);
  declareSetter(0, (Setter<const char*>*) 0, 0);
  declareSetter(0, (Setter<const fixed_string&>*) 0, 0);

#define DUMMY_INST(type) \
  declareAttrib(0, (Getter<type>*) 0, (Setter<type>*) 0, 0)

  DUMMY_INST(int);
  DUMMY_INST(unsigned int);
  DUMMY_INST(unsigned long);
  DUMMY_INST(bool);
  DUMMY_INST(double);
  DUMMY_INST(const char*);
  DUMMY_INST(const fixed_string&);

#undef DUMMY_INST
}

void Tcl::TclItemPkg::declareAction(const char* action_name, Action* action,
                                    const char* usage)
{
  addCommand( new VecActionCmd(interp(), this, makePkgCmdName(action_name),
                               make_shared(action), usage, itemArgn()) );
}


///////////////////////////////////////////////////////////////////////
//
// IoFetcher member definitions and IO commands
//
///////////////////////////////////////////////////////////////////////

Tcl::IoFetcher::IoFetcher()
{
DOTRACE("Tcl::IoFetcher::IoFetcher");
}

Tcl::IoFetcher::~IoFetcher()
{
DOTRACE("Tcl::IoFetcher::~IoFetcher");
}

namespace Tcl
{

class ItemStringifyCmd : public StringifyCmd {
public:
  ItemStringifyCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
    StringifyCmd(pkg->interp(), pkg->makePkgCmdName("stringify"),
                 item_argn ? "item_id" : NULL,
                 item_argn+1),
    itsFetcher(fetcher),
    itsItemArgn(item_argn) {}

protected:
  virtual IO::IoObject& getIO(Tcl::Context& ctx)
  {
    int id = itsItemArgn ? ctx.getIntFromArg(itsItemArgn) : -1;
    return itsFetcher->getIoFromId(id);
  }

private:
  ItemStringifyCmd(const ItemStringifyCmd&);
  ItemStringifyCmd& operator=(const ItemStringifyCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

class ItemDestringifyCmd : public DestringifyCmd {
public:
  ItemDestringifyCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
    DestringifyCmd(pkg->interp(), pkg->makePkgCmdName("destringify"),
                   item_argn ? "item_id string" : "string",
                   item_argn+2),
    itsFetcher(fetcher),
    itsItemArgn(item_argn) {}

protected:
  virtual IO::IoObject& getIO(Tcl::Context& ctx)
  {
    int id = itsItemArgn ? ctx.getIntFromArg(itsItemArgn) : -1;
    return itsFetcher->getIoFromId(id);
  }

private:
  ItemDestringifyCmd(const ItemDestringifyCmd&);
  ItemDestringifyCmd& operator=(const ItemDestringifyCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

class ItemWriteCmd : public WriteCmd {
public:
  ItemWriteCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
    WriteCmd(pkg->interp(), pkg->makePkgCmdName("write"),
             item_argn ? "item_id" : NULL,
             item_argn+1),
    itsFetcher(fetcher),
    itsItemArgn(item_argn) {}

protected:
  virtual IO::IoObject& getIO(Tcl::Context& ctx)
  {
    int id = itsItemArgn ? ctx.getIntFromArg(itsItemArgn) : -1;
    return itsFetcher->getIoFromId(id);
  }

private:
  ItemWriteCmd(const ItemWriteCmd&);
  ItemWriteCmd& operator=(const ItemWriteCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

class ItemReadCmd : public ReadCmd {
public:
  ItemReadCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
    ReadCmd(pkg->interp(), pkg->makePkgCmdName("read"),
             item_argn ? "item_id string" : "string",
             item_argn+2),
    itsFetcher(fetcher),
    itsItemArgn(item_argn) {}

protected:
  virtual IO::IoObject& getIO(Tcl::Context& ctx)
  {
    int id = itsItemArgn ? ctx.getIntFromArg(itsItemArgn) : -1;
    return itsFetcher->getIoFromId(id);
  }

private:
  ItemReadCmd(const ItemReadCmd&);
  ItemReadCmd& operator=(const ItemReadCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

class ItemASRLoadCmd : public ASRLoadCmd {
public:
  ItemASRLoadCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
    ASRLoadCmd(pkg->interp(), pkg->makePkgCmdName("load"),
               item_argn ? "item_id filename" : "filename",
               item_argn+2),
    itsFetcher(fetcher),
    itsItemArgn(item_argn) {}

protected:
  virtual IO::IoObject& getIO(Tcl::Context& ctx)
  {
    int id = itsItemArgn ? ctx.getIntFromArg(itsItemArgn) : -1;
    return itsFetcher->getIoFromId(id);
  }

  virtual const char* getFilename(Tcl::Context& ctx)
  {
    return ctx.getCstringFromArg(itsItemArgn+1);
  }

private:
  ItemASRLoadCmd(const ItemASRLoadCmd&);
  ItemASRLoadCmd& operator=(const ItemASRLoadCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

class ItemASWSaveCmd : public ASWSaveCmd {
public:
  ItemASWSaveCmd(TclItemPkg* pkg, IoFetcher* fetcher, int item_argn) :
    ASWSaveCmd(pkg->interp(), pkg->makePkgCmdName("save"),
               item_argn ? "item_id filename" : "filename",
               item_argn+2),
    itsFetcher(fetcher),
    itsItemArgn(item_argn) {}

protected:
  virtual IO::IoObject& getIO(Tcl::Context& ctx)
  {
    int id = itsItemArgn ? ctx.getIntFromArg(itsItemArgn) : -1;
    return itsFetcher->getIoFromId(id);
  }

  virtual const char* getFilename(Tcl::Context& ctx) {
    return ctx.getCstringFromArg(itsItemArgn+1);
  }

private:
  ItemASWSaveCmd(const ItemASWSaveCmd&);
  ItemASWSaveCmd& operator=(const ItemASWSaveCmd&);

  IoFetcher* itsFetcher;
  int itsItemArgn;
};

} // end namespace Tcl

void Tcl::TclItemPkg::addIoCommands(IoFetcher* fetcher)
{
DOTRACE("Tcl::TclItemPkg::addIoCommands");
  addCommand( new ItemStringifyCmd(this, fetcher, itemArgn()) );
  addCommand( new ItemDestringifyCmd(this, fetcher, itemArgn()) );

  addCommand( new ItemWriteCmd(this, fetcher, itemArgn()) );
  addCommand( new ItemReadCmd(this, fetcher, itemArgn()) );

  addCommand( new ItemASWSaveCmd(this, fetcher, itemArgn()) );
  addCommand( new ItemASRLoadCmd(this, fetcher, itemArgn()) );
}

static const char vcid_tclitempkg_cc[] = "$Header$";
#endif // !TCLITEMPKG_CC_DEFINED
