///////////////////////////////////////////////////////////////////////
//
// iotcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Oct 30 10:00:39 2000
// written: Tue Aug  7 15:22:13 2001
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

#include <fstream.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace
{
  const int ALL = -1; // indicates to read all objects until eof

  Tcl::List loadObjects(const char* file, int num_to_read)
  {
    STD_IO::ifstream ifs(file);
    if (ifs.fail()) { throw Tcl::TclError("unable to open file"); }

    int num_read = 0;

    ifs >> STD_IO::ws;

    Tcl::List result;

    while ( (num_to_read == ALL || num_read < num_to_read)
            && (ifs.peek() != EOF) )
      {
        // allow for whole-line comments between objects beginning with '#'
        if (ifs.peek() == '#')
          {
            ifs.ignore(10000000, '\n');
            continue;
          }

        IO::LegacyReader reader(ifs);

        Ref<IO::IoObject> obj(reader.readRoot(0));

        result.append(obj.id());

        ++num_read;

        ifs >> STD_IO::ws;
      }

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

    for (Tcl::List::Iterator<Ref<IO::IoObject> >
           itr = objids.begin<Ref<IO::IoObject> >(),
           end = objids.end<Ref<IO::IoObject> >();
         itr != end;
         ++itr)
      {
        writer.writeRoot((*itr).get());
      }
  }

  void saveObjectsDefault(Tcl::List objids, const char* filename)
  {
    saveObjects(objids, filename, true);
  }

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

  void dbClear() { ObjDb::theDb().clear(); }
  void dbPurge() { ObjDb::theDb().purge(); }
  void dbRelease(Util::UID id) { ObjDb::theDb().release(id); }
}

namespace IoTcl
{
  class ObjDbPkg : public Tcl::Pkg {

  public:
    ObjDbPkg(Tcl_Interp* interp) :
      Tcl::Pkg(interp, "ObjDb", "$Revision$")
    {
      def( "clear", 0, &dbClear );
      def( "purge", 0, &dbPurge );
      def( "release", 0, &dbRelease );
      def( "loadObjects", "filename num_to_read=-1", &loadObjects );
      def( "loadObjects", "filename", &loadAllObjects );
      def( "saveObjects", "objids filename use_bases=yes", &saveObjects );
      def( "saveObjects", "objids filename", &saveObjectsDefault );
    }

    virtual ~ObjDbPkg()
    {
      ObjDb::theDb().clearOnExit();
    }
  };
}

extern "C"
int Io_Init(Tcl_Interp* interp)
{
DOTRACE("Io_Init");

  Tcl::Pkg* pkg1 = new IoTcl::ObjDbPkg(interp);

  //
  // Obj Package
  //

  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "Obj", "$Revision$");
  Tcl::defGenericObjCmds<Util::Object>(pkg2);

  pkg2->defGetter("refCount", &Util::Object::refCount);
  pkg2->defAction("incrRefCount", &Util::Object::incrRefCount);
  pkg2->defAction("decrRefCount", &Util::Object::decrRefCount);

  pkg2->def( "new", "typename array_size=1", &objNew );
  pkg2->def( "new", "typename", &objNewOne );
  pkg2->def( "delete", "item_id(s)", &objDelete );

  pkg2->eval("proc new {args} { eval Obj::new $args }");
  pkg2->eval("proc delete {args} { eval Obj::delete $args }");

  //
  // IO package
  //

  Tcl::Pkg* pkg3 = new Tcl::Pkg(interp, "IO", "$Revision$");

  Tcl::defGenericObjCmds<IO::IoObject>(pkg3);
  pkg3->defIoCommands();
  pkg3->defGetter("type", &IO::IoObject::ioTypename);

  return Tcl::Pkg::initStatus(pkg1, pkg2, pkg3);
}

static const char vcid_iotcl_cc[] = "$Header$";
#endif // !IOTCL_CC_DEFINED
