///////////////////////////////////////////////////////////////////////
//
// ref.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Oct 27 17:07:31 2000
// written: Wed Mar 19 17:58:52 2003
// $Id$
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

#ifndef REF_CC_DEFINED
#define REF_CC_DEFINED

#include "util/ref.h"

#include "util/error.h"
#include "util/objdb.h"

bool Util::RefHelper::isValidId(Util::UID id)
{
  return ObjDb::theDb().isValidId(id);
}

Util::Object* Util::RefHelper::getCheckedItem(Util::UID id)
{
  return ObjDb::theDb().getCheckedObj(id);
}

void Util::RefHelper::insertItem(Util::Object* obj)
{
  ObjDb::theDb().insertObj(obj);
}

void Util::RefHelper::insertItemWeak(Util::Object* obj)
{
  ObjDb::theDb().insertObjWeak(obj);
}

void Util::RefHelper::throwError(const char* msg)
{
  throw Util::Error(msg);
}

static const char vcid_ref_cc[] = "$Header$";
#endif // !REF_CC_DEFINED
