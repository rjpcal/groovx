///////////////////////////////////////////////////////////////////////
//
// objpkg.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Nov 22 17:06:50 2002
// commit: $Id$
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

#ifndef OBJPKG_CC_DEFINED
#define OBJPKG_CC_DEFINED

#include "objpkg.h"

#include "nub/objdb.h"
#include "nub/object.h"

#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/iter.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using rutz::shared_ptr;

//---------------------------------------------------------------------
//
// ObjCaster
//
//---------------------------------------------------------------------

Tcl::ObjCaster::ObjCaster() {}

Tcl::ObjCaster::~ObjCaster() {}

bool Tcl::ObjCaster::isIdMyType(Nub::UID uid) const
{
  Nub::SoftRef<Nub::Object> item(uid);
  return (item.isValid() && isMyType(item.get()));
}

namespace
{
  int countAll(shared_ptr<Tcl::ObjCaster> caster)
  {
    Nub::ObjDb& theDb = Nub::ObjDb::theDb();
    int count = 0;
    for (Nub::ObjDb::Iterator itr(theDb.objects()); itr.is_valid(); ++itr)
      {
        if (caster->isMyType((*itr).getWeak()))
          ++count;
      }
    return count;
  }

  Tcl::List findAll(shared_ptr<Tcl::ObjCaster> caster)
  {
    Nub::ObjDb& theDb = Nub::ObjDb::theDb();

    Tcl::List result;

    for (Nub::ObjDb::Iterator itr(theDb.objects()); itr.is_valid(); ++itr)
      {
        if (caster->isMyType((*itr).getWeak()))
          result.append((*itr).id());
      }

    return result;
  }

  void removeAll(shared_ptr<Tcl::ObjCaster> caster)
  {
    Nub::ObjDb& theDb = Nub::ObjDb::theDb();
    for (Nub::ObjDb::Iterator itr(theDb.objects());
         itr.is_valid();
         /* increment done in loop body */)
      {
        dbg_eval(3, (*itr)->id());
        dbg_dump(3, *(*itr)->refCounts());

        if (caster->isMyType((*itr).getWeak()) && (*itr)->isUnshared())
          {
            Nub::UID remove_me = (*itr)->id();
            ++itr;
            theDb.remove(remove_me);
          }
        else
          {
            ++itr;
          }
      }
  }

  bool isMyType(shared_ptr<Tcl::ObjCaster> caster, Nub::UID id)
  {
    return caster->isIdMyType(id);
  }

  unsigned int getSizeof(shared_ptr<Tcl::ObjCaster> caster)
  {
    return caster->getSizeof();
  }
}

void Tcl::defGenericObjCmds(Tcl::Pkg* pkg,
                            shared_ptr<Tcl::ObjCaster> caster,
                            const rutz::file_pos& src_pos)
{
DOTRACE("Tcl::defGenericObjCmds");
  pkg->defVec( "is", "objref(s)",
               rutz::bind_first(isMyType, caster), 1, src_pos );
  pkg->def( "countAll", "",
            rutz::bind_first(countAll, caster), src_pos );
  pkg->def( "findAll", "",
            rutz::bind_first(findAll, caster), src_pos );
  pkg->def( "removeAll", "",
            rutz::bind_first(removeAll, caster), src_pos );
  pkg->def( "sizeof", "",
            rutz::bind_first(getSizeof, caster), src_pos );
}

static const char vcid_objpkg_cc[] = "$Header$";
#endif // !OBJPKG_CC_DEFINED
