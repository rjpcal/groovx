///////////////////////////////////////////////////////////////////////
//
// iotcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Oct 30 10:00:39 2000
// written: Wed Jul 18 13:06:30 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOTCL_CC_DEFINED
#define IOTCL_CC_DEFINED

#include "io/io.h"
#include "io/iolegacy.h"

#include "tcl/tclerror.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/objdb.h"
#include "util/objmgr.h"

#ifdef FUNCTIONAL_OK
#  include <algorithm>
#  include <functional>
#endif
#include <fstream.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace IoTcl
{
  const int ALL = -1; // indicates to read all objects until eof

  template <class ReaderType, class Inserter>
  void readBatch(STD_IO::istream& is, int num_to_read,
                 Inserter result_inserter)
    {
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

  Tcl::List loadObjects(const char* file, int num_to_read)
  {
    STD_IO::ifstream ifs(file);
    if (ifs.fail()) { throw Tcl::TclError("unable to open file"); }

    Tcl::List result;

    readBatch<IO::LegacyReader>(ifs, num_to_read, result.appender());

    return result;
  }

  Tcl::List loadAllObjects(const char* file)
  {
    return loadObjects(file, ALL);
  }

  void saveObjects(Tcl::List objids, const char* filename, bool use_bases)
  {
    STD_IO::ofstream ofs(filename);
    if (ofs.fail())
      {
        Tcl::TclError err("error opening file: ");
        err.appendMsg(filename);
        throw err;
      }

    IO::LegacyWriter writer(ofs, use_bases);
    writer.usePrettyPrint(false);
    writeBatch(writer, objids.begin<Util::UID>(), objids.end<Util::UID>());
  }

  void saveObjectsDefault(Tcl::List objids, const char* filename)
  {
    saveObjects(objids, filename, true);
  }
}

namespace Tcl
{

  //
  // objNew
  //

  Tcl::List objNew(const char* type, unsigned int array_size)
  {
    Tcl::List result;

    while (array_size-- > 0)
      {
        WeakRef<Util::Object> item(Util::ObjMgr::newObj(type));
        result.append(item.id());
      }

    return result;
  }

  Tcl::List objNewOne(const char* type)
  {
    return objNew(type, 1);
  }

  //
  // objDelete
  //

  void objDelete(Tcl::List item_ids)
  {
    Tcl::List::Iterator<Util::UID>
      itr = item_ids.begin<Util::UID>(),
      stop = item_ids.end<Util::UID>();
    while (itr != stop)
      {
        ObjDb::theDb().remove(*itr);
        ++itr;
      }
  }

class IoObjectPkg : public Pkg
{
public:
  IoObjectPkg(Tcl_Interp* interp) :
    Pkg(interp, "IO", "$Revision$")
  {
    Pkg::defIoCommands();

    Tcl::defGenericObjCmds<IO::IoObject>(this);

    defGetter("type", &IO::IoObject::ioTypename);
  }
};

class ObjectPkg : public Pkg
{
public:
  ObjectPkg(Tcl_Interp* interp) :
    Pkg(interp, "Obj", "$Revision$")
  {
    Tcl::defGenericObjCmds<Util::Object>(this);

    defGetter("refCount", &Util::Object::refCount);
    defAction("incrRefCount", &Util::Object::incrRefCount);
    defAction("decrRefCount", &Util::Object::decrRefCount);

    def( "new", "typename array_size=1", &Tcl::objNew );
    def( "new", "typename", &Tcl::objNewOne );
    def( "delete", "item_id(s)", &Tcl::objDelete );

    Pkg::eval("proc new {args} { eval Obj::new $args }");
    Pkg::eval("proc delete {args} { eval Obj::delete $args }");

    Pkg::eval("namespace eval IO {\n"
              "proc new {args} { eval Obj::new $args }\n"
              "proc delete {args} { eval Obj::delete $args }\n"
              "}");
  }
};

class ObjDbPkg : public Pkg {
  static void clear() { ObjDb::theDb().clear(); }
  static void purge() { ObjDb::theDb().purge(); }
  static void release(Util::UID id) { ObjDb::theDb().release(id); }

public:
  ObjDbPkg(Tcl_Interp* interp) :
    Pkg(interp, "ObjDb", "$Revision$")
  {
    def( "clear", 0, &ObjDbPkg::clear );
    def( "purge", 0, &ObjDbPkg::purge );
    def( "release", 0, &ObjDbPkg::release );
    def( "loadObjects", "filename num_to_read=-1", &IoTcl::loadObjects );
    def( "loadObjects", "filename", &IoTcl::loadAllObjects );
    def( "saveObjects", "objids filename use_bases=yes", &IoTcl::saveObjects );
    def( "saveObjects", "objids filename", &IoTcl::saveObjectsDefault );

    Pkg::eval("namespace eval IoDb {\n"
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
};

} // end namespace Tcl

extern "C"
int Io_Init(Tcl_Interp* interp)
{
DOTRACE("Io_Init");

  Tcl::Pkg* pkg1 = new Tcl::ObjDbPkg(interp);
  Tcl::Pkg* pkg2 = new Tcl::ObjectPkg(interp);
  Tcl::Pkg* pkg3 = new Tcl::IoObjectPkg(interp);

  return Tcl::Pkg::initStatus(pkg1, pkg2, pkg3);
}

static const char vcid_iotcl_cc[] = "$Header$";
#endif // !IOTCL_CC_DEFINED
