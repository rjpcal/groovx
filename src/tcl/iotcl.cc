///////////////////////////////////////////////////////////////////////
//
// iotcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Oct 30 10:00:39 2000
// written: Thu May 17 11:52:23 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOTCL_CC_DEFINED
#define IOTCL_CC_DEFINED

#include "tcl/ioitempkg.h"

#include "io/io.h"
#include "io/iolegacy.h"
#include "io/iomgr.h"
#include "io/iodb.h"

#include "system/demangle.h"

#include "tcl/stringifycmd.h"
#include "tcl/tclpkg.h"

#include <fstream.h>
#include <typeinfo>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace IoTcl {
  class LoadObjectsCmd;
  class SaveObjectsCmd;

  template <class ReaderType, class Inserter>
  void readBatch(STD_IO::istream& is, int num_to_read,
					  Inserter result_inserter)
	 {
		const int ALL = -1; // indicates to read all objects until eof

		int num_read = 0;

		is >> ws;

		while ( (num_to_read == ALL || num_read < num_to_read)
				  && (is.peek() != EOF) ) {

		  // allow for whole-line comments between objects beginning with '#'
		  if (is.peek() == '#') {
			 is.ignore(10000000, '\n');
			 continue;
		  }

		  ReaderType reader(is);

		  IdItem<IO::IoObject> obj(reader.readRoot(0));

		  *result_inserter = obj.id();
		  ++result_inserter;

		  ++num_read;

		  is >> ws;
		}
	 }

  template <class WriterType, class Iterator>
  void writeBatch(WriterType& writer, Iterator obj_itr, Iterator end)
	 {
		while (obj_itr != end)
		  {
			 IdItem<IO::IoObject> item(*obj_itr);
			 writer.writeRoot(item.get());

			 ++obj_itr;
		  }
	 }
}

//---------------------------------------------------------------------
//
// LoadObjectsCmd --
//
//---------------------------------------------------------------------

class IoTcl::LoadObjectsCmd : public Tcl::TclCmd {
public:
  LoadObjectsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name,
					 "filename ?num_to_read=-1?", 2, 3, false)
  {}

protected:
  virtual void invoke();
};

void IoTcl::LoadObjectsCmd::invoke() {
DOTRACE("IoTcl::LoadObjectsCmd::invoke");
  static const int ALL = -1; // indicates to read all objects until eof

  const char* file        =                             getCstringFromArg(1);
  int         num_to_read =      (objc() < 3) ? ALL   : getIntFromArg(2);

  STD_IO::ifstream ifs(file);
  if (ifs.fail()) { throw Tcl::TclError("unable to open file"); }

  readBatch<IO::LegacyReader>(ifs, num_to_read, resultAppender((int*)0));
}

//---------------------------------------------------------------------
//
// SaveObjectsCmd --
//
//---------------------------------------------------------------------

class IoTcl::SaveObjectsCmd : public Tcl::TclCmd {
public:
  SaveObjectsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 Tcl::TclCmd(interp, cmd_name,
					 "objids filename ?use_bases=yes?", 3, 4)
  {}
protected:
  virtual void invoke() {

	 const char* filename = arg(2).getCstring();

	 bool use_bases    = objc() < 4 ? true : arg(3).getBool();

	 STD_IO::ofstream ofs(filename);
	 if (ofs.fail()) {
		Tcl::TclError err("error opening file: ");
		err.appendMsg(filename);
		throw err;
	 }

	 IO::LegacyWriter writer(ofs, use_bases);
	 writer.usePrettyPrint(false);
	 writeBatch(writer, beginOfArg(1, (int*)0), endOfArg(1, (int*)0));
  }
};

namespace Tcl {

//---------------------------------------------------------------------
//
// TypeCmd
//
//---------------------------------------------------------------------

class IoTypeCmd : public TclItemCmd<IO::IoObject> {
public:
  IoTypeCmd(CTclItemPkg<IO::IoObject>* pkg, const char* cmd_name) :
	 TclItemCmd<IO::IoObject>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 IO::IoObject* p = TclItemCmd<IO::IoObject>::getItem();
	 returnCstring(demangle_cstr(typeid(*p).name()));
  }
};

//---------------------------------------------------------------------
//
// RefCountCmd
//
//---------------------------------------------------------------------

class IoRefCountCmd : public TclItemCmd<IO::IoObject> {
public:
  IoRefCountCmd(CTclItemPkg<IO::IoObject>* pkg, const char* cmd_name) :
	 TclItemCmd<IO::IoObject>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 IO::IoObject* p = TclItemCmd<IO::IoObject>::getItem();
	 returnInt(p->refCount());
  }
};

//---------------------------------------------------------------------
//
// IncrRefCountCmd
//
//---------------------------------------------------------------------

class IoIncrRefCountCmd : public TclItemCmd<IO::IoObject> {
public:
  IoIncrRefCountCmd(CTclItemPkg<IO::IoObject>* pkg, const char* cmd_name) :
	 TclItemCmd<IO::IoObject>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 IO::IoObject* p = TclItemCmd<IO::IoObject>::getItem();
	 p->incrRefCount2();
  }
};

//---------------------------------------------------------------------
//
// DecrRefCountCmd
//
//---------------------------------------------------------------------

class IoDecrRefCountCmd : public TclItemCmd<IO::IoObject> {
public:
  IoDecrRefCountCmd(CTclItemPkg<IO::IoObject>* pkg, const char* cmd_name) :
	 TclItemCmd<IO::IoObject>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 IO::IoObject* p = TclItemCmd<IO::IoObject>::getItem();
	 p->decrRefCount2();
  }
};

//---------------------------------------------------------------------
//
// IoNewCmd
//
//---------------------------------------------------------------------

class IoNewCmd : public TclCmd {
public:
  IoNewCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "typename", 2, 2)
	 {}

protected:
  virtual void invoke() {
	 const char* type = getCstringFromArg(1);

	 IdItem<IO::IoObject> item(IO::IoMgr::newIO(type));

	 returnInt(item.id());
  }
};

//---------------------------------------------------------------------
//
// IoDeleteCmd
//
//---------------------------------------------------------------------

class IoDeleteCmd : public TclCmd {
public:
  IoDeleteCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "item_id(s)", 2, 2)
	 {}

protected:
  virtual void invoke() {
	 Tcl::ListIterator<int>
		itr = beginOfArg(1, (int*)0),
		stop = endOfArg(1, (int*)0);
	 while (itr != stop)
		{
		  IoDb::theDb().remove(*itr);
		  ++itr;
		}
  }
};

class IoObjectPkg : public IoItemPkg<IO::IoObject>,
                  public IoFetcher
{
public:
  IoObjectPkg(Tcl_Interp* interp) :
	 IoItemPkg<IO::IoObject>(interp, "IO", "$Revision$")
  {
	 TclItemPkg::addIoCommands(this);

	 addCommand( new IoTypeCmd(this, TclPkg::makePkgCmdName("type")) );
	 addCommand( new IoRefCountCmd(this, TclPkg::makePkgCmdName("refCount")));
	 addCommand( new IoIncrRefCountCmd(this,
                        TclPkg::makePkgCmdName("incrRefCount")));
	 addCommand( new IoDecrRefCountCmd(this,
								TclPkg::makePkgCmdName("decrRefCount")));

	 addCommand( new IoNewCmd(interp, TclPkg::makePkgCmdName("new")));
	 addCommand( new IoDeleteCmd(interp, TclPkg::makePkgCmdName("delete")));

	 TclPkg::eval("proc new {args} { eval IO::new $args }");
	 TclPkg::eval("proc delete {args} { eval IO::delete $args }");
  }

  virtual IO::IoObject& getIoFromId(int id) {
	 return dynamic_cast<IO::IoObject&>( *(getCItemFromId(id)) );
  }
};

class IoDbPkg : public CTclItemPkg<IoDb> {
public:
  IoDbPkg(Tcl_Interp* interp) :
	 CTclItemPkg<IoDb>(interp, "IoDb", "$Revision$", 0)
  {
	 declareCAction("clear", &IoDb::clear);
	 declareCAction("purge", &IoDb::purge);
	 addCommand( new IoTcl::LoadObjectsCmd(interp, "IoDb::loadObjects") );
	 addCommand( new IoTcl::SaveObjectsCmd(interp, "IoDb::saveObjects") );
  }

  virtual ~IoDbPkg()
    {
		IoDb::theDb().clearOnExit();
	 }

  IoDb* getCItemFromId(int)
    { return &(IoDb::theDb()); }
};

} // end namespace Tcl

extern "C"
int Io_Init(Tcl_Interp* interp) {
DOTRACE("Io_Init");

  Tcl::TclPkg* pkg1 = new Tcl::IoDbPkg(interp);
  Tcl::TclPkg* pkg2 = new Tcl::IoObjectPkg(interp);

  return pkg1->combineStatus(pkg2->initStatus());
}

static const char vcid_iotcl_cc[] = "$Header$";
#endif // !IOTCL_CC_DEFINED
