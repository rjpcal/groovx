///////////////////////////////////////////////////////////////////////
//
// iotcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Oct 30 10:00:39 2000
// written: Tue Jun 19 15:11:26 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOTCL_CC_DEFINED
#define IOTCL_CC_DEFINED

#include "tcl/genericobjpkg.h"

#include "io/io.h"
#include "io/iolegacy.h"

#include "system/demangle.h"

#include "tcl/stringifycmd.h"
#include "tcl/tclpkg.h"

#include "util/objdb.h"
#include "util/objmgr.h"

#ifdef FUNCTIONAL_OK
#  include <algorithm>
#  include <functional>
#endif
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

      is >> STD_IO::ws;

      while ( (num_to_read == ALL || num_read < num_to_read)
              && (is.peek() != EOF) ) {

        // allow for whole-line comments between objects beginning with '#'
        if (is.peek() == '#') {
          is.ignore(10000000, '\n');
          continue;
        }

        ReaderType reader(is);

        Ref<IO::IoObject> obj(reader.readRoot(0));

        *result_inserter = obj.id();
        ++result_inserter;

        ++num_read;

        is >> STD_IO::ws;
      }
    }

  template <class WriterType, class Iterator>
  void writeBatch(WriterType& writer, Iterator obj_itr, Iterator end)
    {
      while (obj_itr != end)
        {
          Ref<IO::IoObject> item(*obj_itr);
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
// ObjNewCmd
//
//---------------------------------------------------------------------

class ObjNewCmd : public TclCmd {
public:
  ObjNewCmd(Tcl_Interp* interp, const char* cmd_name) :
    TclCmd(interp, cmd_name, "typename ?array_size=1?", 2, 3)
    {}

protected:
  virtual void invoke() {
    const char* type = getCstringFromArg(1);

    if (objc() < 3)
      {
        WeakRef<Util::Object> item(Util::ObjMgr::newObj(type));
        returnInt(item.id());
      }
    else
      {
        int array_size = getIntFromArg(2);
        while (array_size-- > 0)
          {
            WeakRef<Util::Object> item(Util::ObjMgr::newObj(type));
            lappendVal(item.id());
          }
      }
  }
};

//---------------------------------------------------------------------
//
// ObjDeleteCmd
//
//---------------------------------------------------------------------

class ObjDeleteCmd : public TclCmd {
public:
  ObjDeleteCmd(Tcl_Interp* interp, const char* cmd_name) :
    TclCmd(interp, cmd_name, "item_id(s)", 2, 2)
    {}

protected:
  virtual void invoke() {
#ifndef FUNCTIONAL_OK
    Tcl::ListIterator<int>
      itr = beginOfArg(1, (int*)0),
      stop = endOfArg(1, (int*)0);
    while (itr != stop)
      {
        ObjDb::theDb().remove(*itr);
        ++itr;
      }

#else
    std::for_each(beginOfArg<int>(1), endOfArg<int>(1),
                  std::bind1st(std::mem_fun(&ObjDb::remove), &ObjDb::theDb()));
#endif
  }
};

class IoObjectPkg : public GenericObjPkg<IO::IoObject>,
                    public IoFetcher
{
public:
  IoObjectPkg(Tcl_Interp* interp) :
    GenericObjPkg<IO::IoObject>(interp, "IO", "$Revision$")
  {
    TclItemPkg::addIoCommands(this);

    declareCGetter("type", &IO::IoObject::ioTypename);
  }

  virtual IO::IoObject& getIoFromId(int id) {
    return dynamic_cast<IO::IoObject&>( *(getCItemFromId(id)) );
  }
};

class ObjectPkg : public GenericObjPkg<Util::Object>
{
public:
  ObjectPkg(Tcl_Interp* interp) :
    GenericObjPkg<Util::Object>(interp, "Obj", "$Revision$")
  {
    declareCGetter("refCount", &Util::Object::refCount);
    declareCAction("incrRefCount", &Util::Object::incrRefCount);
    declareCAction("decrRefCount", &Util::Object::decrRefCount);

    addCommand( new ObjNewCmd(interp, TclPkg::makePkgCmdName("new")));
    addCommand( new ObjDeleteCmd(interp, TclPkg::makePkgCmdName("delete")));

    TclPkg::eval("proc new {args} { eval Obj::new $args }");
    TclPkg::eval("proc delete {args} { eval Obj::delete $args }");

    TclPkg::eval("namespace eval IO {\n"
                 "proc new {args} { eval Obj::new $args }\n"
                 "proc delete {args} { eval Obj::delete $args }\n"
                 "}");
  }
};

class ObjDbPkg : public CTclItemPkg<ObjDb> {
public:
  ObjDbPkg(Tcl_Interp* interp) :
    CTclItemPkg<ObjDb>(interp, "ObjDb", "$Revision$", 0)
  {
    declareCAction("clear", &ObjDb::clear);
    declareCAction("purge", &ObjDb::purge);
    declareCSetter("release", &ObjDb::release);
    addCommand( new IoTcl::LoadObjectsCmd(interp, "ObjDb::loadObjects") );
    addCommand( new IoTcl::SaveObjectsCmd(interp, "ObjDb::saveObjects") );

    TclPkg::eval("namespace eval IoDb {\n"
                 "  proc clear {args} { eval ObjDb::clear $args }\n"
                 "  proc purge {args} { eval ObjDb::purge $args }\n"
                 "  proc release {args} { eval ObjDb::release $args }\n"
                 "  proc loadObjects {args} { eval ObjDb::loadObjects $args }\n"
                 "  proc saveObjects {args} { eval ObjDb::saveObjects $args }\n"
                 "}\n");
  }

  virtual ~ObjDbPkg()
    {
      ObjDb::theDb().clearOnExit();
    }

  ObjDb* getCItemFromId(int)
    { return &(ObjDb::theDb()); }
};

} // end namespace Tcl

extern "C"
int Io_Init(Tcl_Interp* interp) {
DOTRACE("Io_Init");

  Tcl::TclPkg* pkg1 = new Tcl::ObjDbPkg(interp);
  Tcl::TclPkg* pkg2 = new Tcl::ObjectPkg(interp);
  Tcl::TclPkg* pkg3 = new Tcl::IoObjectPkg(interp);

  return pkg1->combineStatus(pkg2->combineStatus(pkg3->initStatus()));
}

static const char vcid_iotcl_cc[] = "$Header$";
#endif // !IOTCL_CC_DEFINED
