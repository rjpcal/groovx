///////////////////////////////////////////////////////////////////////
//
// objpkg.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Nov 22 17:06:50 2002
// commit: $Id$
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

#ifndef OBJPKG_CC_DEFINED
#define OBJPKG_CC_DEFINED

#include "tcl/objpkg.h"

#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/iter.h"
#include "util/objdb.h"
#include "util/object.h"

#include "util/trace.h"
#include "util/debug.h"

//---------------------------------------------------------------------
//
// ObjCaster
//
//---------------------------------------------------------------------

Tcl::ObjCaster::ObjCaster() {}

Tcl::ObjCaster::~ObjCaster() {}

bool Tcl::ObjCaster::isIdMyType(Util::UID uid)
{
  SoftRef<Util::Object> item(uid);
  return (item.isValid() && isMyType(item.get()));
}

namespace
{
  int countAll(shared_ptr<Tcl::ObjCaster> caster)
  {
    ObjDb& theDb = ObjDb::theDb();
    int count = 0;
    for (ObjDb::Iterator itr(theDb.objects()); itr.isValid(); ++itr)
      {
        if (caster->isMyType((*itr).getWeak()))
          ++count;
      }
    return count;
  }

  Tcl::List findAll(shared_ptr<Tcl::ObjCaster> caster)
  {
    ObjDb& theDb = ObjDb::theDb();

    Tcl::List result;

    for (ObjDb::Iterator itr(theDb.objects()); itr.isValid(); ++itr)
      {
        if (caster->isMyType((*itr).getWeak()))
          result.append((*itr).id());
      }

    return result;
  }

  void removeAll(shared_ptr<Tcl::ObjCaster> caster)
  {
    ObjDb& theDb = ObjDb::theDb();
    for (ObjDb::Iterator itr(theDb.objects());
         itr.isValid();
         /* increment done in loop body */)
      {
        dbgEval(3, (*itr)->id());
        dbgEval(3, (*itr)->refCounts()->strongCount());
        dbgEvalNL(3, (*itr)->refCounts()->weakCount());

        if (caster->isMyType((*itr).getWeak()) && (*itr)->isUnshared())
          {
            Util::UID remove_me = (*itr)->id();
            ++itr;
            theDb.remove(remove_me);
          }
        else
          {
            ++itr;
          }
      }
  }

  bool isMyType(shared_ptr<Tcl::ObjCaster> caster, Util::UID id)
  {
    return caster->isIdMyType(id);
  }
}

void Tcl::defGenericObjCmds(Tcl::Pkg* pkg, shared_ptr<Tcl::ObjCaster> caster)
{
DOTRACE("Tcl::defGenericObjCmds");
  pkg->defVec( "is", "item_id(s)", Util::bindFirst(isMyType, caster) );
  pkg->def( "countAll", "", Util::bindFirst(countAll, caster) );
  pkg->def( "findAll", "", Util::bindFirst(findAll, caster) );
  pkg->def( "removeAll", "", Util::bindFirst(removeAll, caster) );
}

static const char vcid_objpkg_cc[] = "$Header$";
#endif // !OBJPKG_CC_DEFINED
