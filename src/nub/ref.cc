///////////////////////////////////////////////////////////////////////
//
// ref.cc
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Oct 27 17:07:31 2000
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

#ifndef REF_CC_DEFINED
#define REF_CC_DEFINED

#include "ref.h"

#include "nub/objdb.h"

#include "util/error.h"
#include "util/demangle.h"

bool Nub::Detail::isValidId(Nub::UID id) throw()
{
  return Nub::ObjDb::theDb().isValidId(id);
}

Nub::Object* Nub::Detail::getCheckedItem(Nub::UID id)
{
  return Nub::ObjDb::theDb().getCheckedObj(id);
}

void Nub::Detail::insertItem(Nub::Object* obj)
{
  Nub::ObjDb::theDb().insertObj(obj);
}

void Nub::Detail::insertItemWeak(Nub::Object* obj)
{
  Nub::ObjDb::theDb().insertObjWeak(obj);
}

void Nub::Detail::throwRefNull(const std::type_info& info,
                               const rutz::file_pos& pos)
{
  throw rutz::error(rutz::fstring("attempted to construct a Ref<",
                                  rutz::demangled_name(info),
                                  "> with a null pointer"),
                    pos);
}

void Nub::Detail::throwRefUnshareable(const std::type_info& info,
                                      const rutz::file_pos& pos)
{
  throw rutz::error(rutz::fstring("attempted to construct a Ref<",
                                  rutz::demangled_name(info),
                                  "> with an unshareable object"),
                    pos);
}

void Nub::Detail::throwSoftRefInvalid(const std::type_info& info,
                                      const rutz::file_pos& pos)
{
  throw rutz::error(rutz::fstring("attempted to access invalid object "
                                  "in SoftRef<", rutz::demangled_name(info), ">"),
                    pos);
}

static const char vcid_ref_cc[] = "$Header$";
#endif // !REF_CC_DEFINED
