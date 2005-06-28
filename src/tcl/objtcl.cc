///////////////////////////////////////////////////////////////////////
//
// objtcl.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 14 16:24:33 2002
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_TCL_OBJTCL_CC_UTC20050626084018_DEFINED
#define GROOVX_TCL_OBJTCL_CC_UTC20050626084018_DEFINED

#include "tcl/objtcl.h"

#include "nub/objdb.h"
#include "nub/objmgr.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclsafeinterp.h"

#include "rutz/demangle.h"

#include "rutz/trace.h"

using Nub::SoftRef;
using Nub::Object;

namespace
{
  void dbClear() { Nub::ObjDb::theDb().clear(); }
  void dbPurge() { Nub::ObjDb::theDb().purge(); }
  void dbRelease(Nub::UID id) { Nub::ObjDb::theDb().release(id); }
  void dbClearOnExit() { Nub::ObjDb::theDb().clearOnExit(); }

  // This is just here to select between the const char* +
  // rutz::fstring versions of newObj().
  SoftRef<Object> objNew(const char* type)
  {
    return Nub::ObjMgr::newObj(type);
  }

  SoftRef<Object> objNewArgs(const char* type,
                             Tcl::List init_args,
                             Tcl::Interp interp)
  {
    SoftRef<Object> obj(Nub::ObjMgr::newObj(type));

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
        SoftRef<Object> item(Nub::ObjMgr::newObj(type));
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

  void arrowDispatch(Tcl::Context& ctx)
  {
    /* old implementation was this:

       pkg->eval("proc ::-> {args} {\n"
                 "  set ids [lindex $args 0]\n"
                 "  set namesp [Obj::type [lindex $ids 0]]\n"
                 "  set cmd [lreplace $args 0 1 [lindex $args 1] $ids]\n"
                 "  namespace eval $namesp $cmd\n"
                 "}");

       but the problem was that it involved a string conversion cycle
       of the trailing args, which meant that we lost the internal rep
    */

    // e.g.      "-> {3 4} foo 4.5"
    // becomes   "Namesp::foo {3 4} 4.5"

    if (ctx.objc() < 3)
      throw rutz::error("bad objc", SRC_POS);

    Tcl_Obj* const* origargs = ctx.getRawArgs();

    Tcl::List objrefs(origargs[1]);

    const rutz::fstring namesp =
      objrefs.get<SoftRef<Object> >(0)->objTypename();

    rutz::fstring origcmdname = ctx.getValFromArg<rutz::fstring>(2);

    rutz::fstring newcmdname(namesp, "::", origcmdname);

    Tcl::List newargs;

    newargs.append(newcmdname);
    newargs.append(objrefs);

    for (unsigned int i = 3; i < ctx.objc(); ++i)
      {
        newargs.append(origargs[i]);
      }

    ctx.interp().eval(newargs.asObj());
  }
}

extern "C"
int Objdb_Init(Tcl_Interp* interp)
{
GVX_TRACE("Objdb_Init");

  GVX_PKG_CREATE(pkg, interp, "ObjDb", "4.$Revision$");

  pkg->onExit( &dbClearOnExit );

  pkg->def( "clear", 0, &dbClear, SRC_POS );
  pkg->def( "purge", 0, &dbPurge, SRC_POS );
  pkg->def( "release", 0, &dbRelease, SRC_POS );

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Obj_Init(Tcl_Interp* interp)
{
GVX_TRACE("Obj_Init");

  GVX_PKG_CREATE(pkg, interp, "Obj", "4.$Revision$");
  Tcl::defGenericObjCmds<Object>(pkg, SRC_POS);

  pkg->defGetter("refCount", &Object::dbg_RefCount, SRC_POS);
  pkg->defGetter("weakRefCount", &Object::dbg_WeakRefCount, SRC_POS);
  pkg->defAction("incrRefCount", &Object::incrRefCount, SRC_POS);
  pkg->defAction("decrRefCount", &Object::decrRefCount, SRC_POS);

  pkg->defGetter( "type", &Object::objTypename, SRC_POS );
  pkg->defGetter( "realType", &Object::realTypename, SRC_POS );

  pkg->def( "new", "typename", &objNew, SRC_POS );
  pkg->def( "new", "typename {cmd1 arg1 cmd2 arg2 ...}",
            rutz::bind_last(&objNewArgs, Tcl::Interp(interp)),
            SRC_POS );
  pkg->def( "newarr", "typename array_size=1", &objNewArr, SRC_POS );
  pkg->def( "delete", "objref(s)", &objDelete, SRC_POS );

  pkg->defRaw( "::->", Tcl::ArgSpec(2).max(1000),
               "objref(s) cmdname ?arg1 arg2 ...?",
               &arrowDispatch, SRC_POS );

  pkg->namespaceAlias("::", "new");
  pkg->namespaceAlias("::", "newarr");
  pkg->namespaceAlias("::", "delete");

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_tcl_objtcl_cc_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_OBJTCL_CC_UTC20050626084018_DEFINED
