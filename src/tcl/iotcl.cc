///////////////////////////////////////////////////////////////////////
//
// iotcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Oct 30 10:00:39 2000
// written: Thu Jan 31 14:45:14 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOTCL_CC_DEFINED
#define IOTCL_CC_DEFINED

#include "tcl/iotcl.h"

#include "io/io.h"
#include "io/iolegacy.h"
#include "io/ioutil.h"

#include "tcl/tclcode.h"
#include "tcl/tclerror.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"
#include "tcl/tclsafeinterp.h"

#include "util/objdb.h"
#include "util/objmgr.h"

#include <fstream>
#include <typeinfo>

#include "util/trace.h"

void Tcl::defIoCommands(Tcl::Pkg* pkg)
{
DOTRACE("Tcl::defIoCommands");
  pkg->defVec( "stringify", "item_id(s)", IO::stringify );
  pkg->defVec( "destringify", "item_id(s) string(s)", IO::destringify );

  pkg->defVec( "write", "item_id(s)", IO::write );
  pkg->defVec( "read", "item_id(s) string(s)", IO::read );

  pkg->def( "save", "item_id filename", IO::saveASW );
  pkg->def( "load", "item_id filename", IO::loadASR );
}

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

  void saveObjects(Tcl::List objids, const char* filename, bool use_bases)
  {
    STD_IO::ofstream ofs(filename);
    if (ofs.fail())
      {
        throw Tcl::TclError(fstring("error opening file: ", filename));
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

  SoftRef<Util::Object> objNew(const char* type)
  {
    return SoftRef<Util::Object>(Util::ObjMgr::newObj(type));
  }

  SoftRef<Util::Object> objNewArgs(const char* type, Tcl::List init_args,
                                   Tcl::Interp interp)
  {
    SoftRef<Util::Object> obj(Util::ObjMgr::newObj(type));

    for(unsigned int i = 0; i+1 < init_args.length(); i+=2)
      {
        Tcl::List cmd_str;
        cmd_str.append("::->");
        cmd_str.append(obj.id());
        cmd_str.append(init_args[i]);
        cmd_str.append(init_args[i+1]);
        Tcl::Code cmd(cmd_str.asObj(), Tcl::Code::THROW_EXCEPTION);
        cmd.invoke(interp);
      }

    return obj;
  }

  Tcl::List objNewArr(const char* type, unsigned int array_size)
  {
    Tcl::List result;

    while (array_size-- > 0)
      {
        SoftRef<Util::Object> item(Util::ObjMgr::newObj(type));
        result.append(item.id());
      }

    return result;
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

  fstring objType(Util::SoftRef<Util::Object> obj)
  {
    return typeid(*obj).name();
  }

  void dbClear() { ObjDb::theDb().clear(); }
  void dbPurge() { ObjDb::theDb().purge(); }
  void dbRelease(Util::UID id) { ObjDb::theDb().release(id); }
}

namespace IoTcl
{
  class ObjDbPkg : public Tcl::Pkg
  {

  public:
    ObjDbPkg(Tcl_Interp* interp) :
      Tcl::Pkg(interp, "ObjDb", "$Revision$")
    {
      def( "clear", 0, &dbClear );
      def( "purge", 0, &dbPurge );
      def( "release", 0, &dbRelease );
      def( "loadObjects", "filename num_to_read=-1", &loadObjects );
      def( "loadObjects", "filename", Util::bindLast(&loadObjects, ALL) );
      def( "saveObjects", "objids filename use_bases=yes", &saveObjects );
      def( "saveObjects", "objids filename",
           Util::bindLast(&saveObjects, true) );
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

  pkg2->defVec( "type", "item_id(s)", &objType );

  pkg2->def( "new", "typename", &objNew );
  pkg2->def( "new", "typename {cmd1 arg1 cmd2 arg2 ...}",
             Util::bindLast(&objNewArgs, Tcl::Interp(interp)) );
  pkg2->def( "newarr", "typename array_size=1", &objNewArr );
  pkg2->def( "delete", "item_id(s)", &objDelete );

  pkg2->eval("proc new {args} { eval Obj::new $args }");
  pkg2->eval("proc newarr {args} { eval Obj::newarr $args }");
  pkg2->eval("proc delete {args} { eval Obj::delete $args }");

  //
  // IO package
  //

  Tcl::Pkg* pkg3 = new Tcl::Pkg(interp, "IO", "$Revision$");

  Tcl::defGenericObjCmds<IO::IoObject>(pkg3);
  Tcl::defIoCommands(pkg3);
  pkg3->defGetter("type", &IO::IoObject::ioTypename);

  pkg3->eval("proc ::-> {args} {\n"
             "set ids [lindex $args 0]\n"
             "set namesp [IO::type [lindex $ids 0]]\n"
             "set cmd [lreplace $args 0 1 [lindex $args 1] $ids]\n"
             "namespace eval $namesp $cmd\n"
             "}");

  return Tcl::Pkg::initStatus(pkg1, pkg2, pkg3);
}

static const char vcid_iotcl_cc[] = "$Header$";
#endif // !IOTCL_CC_DEFINED
