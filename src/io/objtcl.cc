///////////////////////////////////////////////////////////////////////
//
// objtcl.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 14 16:24:33 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJTCL_CC_DEFINED
#define OBJTCL_CC_DEFINED

#include "io/io.h"
#include "io/iolegacy.h"

#include "nub/objdb.h"
#include "nub/objmgr.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclsafeinterp.h"

#include "util/demangle.h"

#include <fstream>
#include <typeinfo>

#include "util/trace.h"

using Nub::Ref;
using Nub::SoftRef;

namespace
{
  const int ALL = -1; // indicates to read all objects until eof

  Tcl::List loadObjects(const char* file, int num_to_read)
  {
    STD_IO::ifstream ifs(file);
    if (ifs.fail())
      {
        throw rutz::error("unable to open file", SRC_POS);
      }

    int num_read = 0;

    ifs >> STD_IO::ws;

    Tcl::List result;

    while ( (num_to_read == ALL || num_read < num_to_read)
            && (ifs.peek() != EOF) )
      {
        // allow for whole-line comments between objects beginning
        // with '#'
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

  void saveObjects(Tcl::List objids, const char* filename,
                   bool use_bases)
  {
    STD_IO::ofstream ofs(filename);
    if (ofs.fail())
      {
        throw rutz::error(rutz::fstring("error opening file: ",
                                        filename), SRC_POS);
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

  void dbClear() { Nub::ObjDb::theDb().clear(); }
  void dbPurge() { Nub::ObjDb::theDb().purge(); }
  void dbRelease(Nub::UID id) { Nub::ObjDb::theDb().release(id); }
  void dbClearOnExit() { Nub::ObjDb::theDb().clearOnExit(); }

  // This is just here to select between the const char* +
  // rutz::fstring versions of newObj().
  SoftRef<Nub::Object> objNew(const char* type)
  {
    return Nub::ObjMgr::newObj(type);
  }

  SoftRef<Nub::Object> objNewArgs(const char* type,
                                  Tcl::List init_args,
                                  Tcl::Interp interp)
  {
    SoftRef<Nub::Object> obj(Nub::ObjMgr::newObj(type));

    for (unsigned int i = 0; i+1 < init_args.length(); i+=2)
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
        SoftRef<Nub::Object> item(Nub::ObjMgr::newObj(type));
        result.append(item.id());
      }

    return result;
  }

  void objDelete(Tcl::List objrefs)
  {
    Tcl::List::Iterator<Nub::UID>
      itr = objrefs.begin<Nub::UID>(),
      stop = objrefs.end<Nub::UID>();
    while (itr != stop)
      {
        Nub::ObjDb::theDb().remove(*itr);
        ++itr;
      }
  }
}

extern "C"
int Objdb_Init(Tcl_Interp* interp)
{
DOTRACE("Objdb_Init");

  PKG_CREATE(interp, "ObjDb", "$Revision$");

  pkg->onExit( &dbClearOnExit );

  pkg->def( "clear", 0, &dbClear, SRC_POS );
  pkg->def( "purge", 0, &dbPurge, SRC_POS );
  pkg->def( "release", 0, &dbRelease, SRC_POS );
  pkg->def( "loadObjects", "filename num_to_read=-1", &loadObjects, SRC_POS );
  pkg->def( "loadObjects", "filename", rutz::bind_last(&loadObjects, ALL), SRC_POS );
  pkg->def( "saveObjects", "objids filename use_bases=yes", &saveObjects, SRC_POS );
  pkg->def( "saveObjects", "objids filename",
            rutz::bind_last(&saveObjects, true), SRC_POS );

  PKG_RETURN;
}

extern "C"
int Obj_Init(Tcl_Interp* interp)
{
DOTRACE("Obj_Init");

  PKG_CREATE(interp, "Obj", "$Revision$");
  Tcl::defGenericObjCmds<Nub::Object>(pkg, SRC_POS);

  pkg->defGetter("refCount", &Nub::Object::dbg_RefCount, SRC_POS);
  pkg->defAction("incrRefCount", &Nub::Object::incrRefCount, SRC_POS);
  pkg->defAction("decrRefCount", &Nub::Object::decrRefCount, SRC_POS);

  pkg->defGetter( "type", "objref(s)", &Nub::Object::objTypename, SRC_POS );
  pkg->defGetter( "realType", "objref(s)", &Nub::Object::realTypename, SRC_POS );

  pkg->def( "new", "typename", &objNew, SRC_POS );
  pkg->def( "new", "typename {cmd1 arg1 cmd2 arg2 ...}",
            rutz::bind_last(&objNewArgs, Tcl::Interp(interp)),
            SRC_POS );
  pkg->def( "newarr", "typename array_size=1", &objNewArr, SRC_POS );
  pkg->def( "delete", "objref(s)", &objDelete, SRC_POS );

  pkg->namespaceAlias("::", "new");
  pkg->namespaceAlias("::", "newarr");
  pkg->namespaceAlias("::", "delete");

  pkg->eval("proc ::-> {args} {\n"
            "  set ids [lindex $args 0]\n"
            "  set namesp [Obj::type [lindex $ids 0]]\n"
            "  set cmd [lreplace $args 0 1 [lindex $args 1] $ids]\n"
            "  namespace eval $namesp $cmd\n"
            "}");

  PKG_RETURN;
}

static const char vcid_objtcl_cc[] = "$Header$";
#endif // !OBJTCL_CC_DEFINED
