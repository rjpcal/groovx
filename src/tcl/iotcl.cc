///////////////////////////////////////////////////////////////////////
//
// iotcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Oct 30 10:00:39 2000
// written: Mon Jul 16 13:00:10 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOTCL_CC_DEFINED
#define IOTCL_CC_DEFINED

#include "io/io.h"
#include "io/iolegacy.h"

#include "tcl/tclerror.h"
#include "tcl/tclitempkg.h"
#include "tcl/tcllistobj.h"

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

class IoObjectPkg : public TclItemPkg
{
public:
  IoObjectPkg(Tcl_Interp* interp) :
    TclItemPkg(interp, "IO", "$Revision$")
  {
    TclItemPkg::addIoCommands();

    Tcl::defGenericObjCmds<IO::IoObject>(this);

    defGetter("type", &IO::IoObject::ioTypename);
  }
};

class ObjectPkg : public TclItemPkg
{
public:
  ObjectPkg(Tcl_Interp* interp) :
    TclItemPkg(interp, "Obj", "$Revision$")
  {
    Tcl::defGenericObjCmds<Util::Object>(this);

    defGetter("refCount", &Util::Object::refCount);
    defAction("incrRefCount", &Util::Object::incrRefCount);
    defAction("decrRefCount", &Util::Object::decrRefCount);

    def( &Tcl::objNew, "new", "typename array_size=1" );
    def( &Tcl::objNewOne, "new", "typename" );
    def( &Tcl::objDelete, "delete", "item_id(s)" );

    TclPkg::eval("proc new {args} { eval Obj::new $args }");
    TclPkg::eval("proc delete {args} { eval Obj::delete $args }");

    TclPkg::eval("namespace eval IO {\n"
                 "proc new {args} { eval Obj::new $args }\n"
                 "proc delete {args} { eval Obj::delete $args }\n"
                 "}");
  }
};

class ObjDbPkg : public TclItemPkg {
  static void clear() { ObjDb::theDb().clear(); }
  static void purge() { ObjDb::theDb().purge(); }
  static void release(Util::UID id) { ObjDb::theDb().release(id); }

public:
  ObjDbPkg(Tcl_Interp* interp) :
    TclItemPkg(interp, "ObjDb", "$Revision$")
  {
    def( &ObjDbPkg::clear, "clear", 0 );
    def( &ObjDbPkg::purge, "purge", 0 );
    def( &ObjDbPkg::release, "release", 0 );
    def( &IoTcl::loadObjects, "loadObjects", "filename num_to_read=-1" );
    def( &IoTcl::loadAllObjects, "loadObjects", "filename" );
    def( &IoTcl::saveObjects,
         "saveObjects", "objids filename use_bases=yes" );
    def( &IoTcl::saveObjectsDefault,
         "saveObjects", "objids filename" );

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
};

} // end namespace Tcl

extern "C"
int Io_Init(Tcl_Interp* interp)
{
DOTRACE("Io_Init");

  Tcl::TclPkg* pkg1 = new Tcl::ObjDbPkg(interp);
  Tcl::TclPkg* pkg2 = new Tcl::ObjectPkg(interp);
  Tcl::TclPkg* pkg3 = new Tcl::IoObjectPkg(interp);

  return pkg1->combineStatus(pkg2->combineStatus(pkg3->initStatus()));
}

static const char vcid_iotcl_cc[] = "$Header$";
#endif // !IOTCL_CC_DEFINED
