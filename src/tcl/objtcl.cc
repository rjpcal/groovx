///////////////////////////////////////////////////////////////////////
//
// objtcl.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 14 16:24:33 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJTCL_CC_DEFINED
#define OBJTCL_CC_DEFINED

#include "io/io.h"
#include "io/iolegacy.h"

#include "system/demangle.h"

#include "tcl/objpkg.h"
#include "tcl/tclerror.h"
#include "tcl/tclpkg.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclsafeinterp.h"

#include "util/objdb.h"
#include "util/objmgr.h"

#include <fstream>
#include <typeinfo>

#include "util/trace.h"

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

  void dbClear() { ObjDb::theDb().clear(); }
  void dbPurge() { ObjDb::theDb().purge(); }
  void dbRelease(Util::UID id) { ObjDb::theDb().release(id); }

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
        interp.eval(cmd_str.asObj());
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
}

extern "C"
int Objdb_Init(Tcl_Interp* interp)
{
DOTRACE("Objdb_Init");

  Tcl::Pkg* pkg = new ObjDbPkg(interp);

  return pkg->initStatus();
}

extern "C"
int Obj_Init(Tcl_Interp* interp)
{
DOTRACE("Obj_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Obj", "$Revision$");
  Tcl::defGenericObjCmds<Util::Object>(pkg);

  pkg->defGetter("refCount", &Util::Object::refCount);
  pkg->defAction("incrRefCount", &Util::Object::incrRefCount);
  pkg->defAction("decrRefCount", &Util::Object::decrRefCount);

  pkg->defVec( "type", "item_id(s)", &Util::Object::objTypename );
  pkg->defVec( "realType", "item_id(s)", &Util::Object::realTypename );

  pkg->def( "new", "typename", &objNew );
  pkg->def( "new", "typename {cmd1 arg1 cmd2 arg2 ...}",
             Util::bindLast(&objNewArgs, Tcl::Interp(interp)) );
  pkg->def( "newarr", "typename array_size=1", &objNewArr );
  pkg->def( "delete", "item_id(s)", &objDelete );

  pkg->eval("proc new {args} { eval Obj::new $args }");
  pkg->eval("proc newarr {args} { eval Obj::newarr $args }");
  pkg->eval("proc delete {args} { eval Obj::delete $args }");

  return pkg->initStatus();
}

static const char vcid_objtcl_cc[] = "$Header$";
#endif // !OBJTCL_CC_DEFINED
