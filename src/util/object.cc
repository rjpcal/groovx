///////////////////////////////////////////////////////////////////////
//
// object.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jun  5 10:26:14 2001
// written: Wed Mar 19 17:58:53 2003
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

#ifndef OBJECT_CC_DEFINED
#define OBJECT_CC_DEFINED

#include "util/object.h"

#include "util/trace.h"

namespace
{
  Util::UID idCounter = 0;
}

Util::Object::Object() : itsId(++idCounter)
{
DOTRACE("Util::Object::Object");
}

Util::Object::~Object()
{
DOTRACE("Util::Object::~Object");
}

Util::UID Util::Object::id() const { return itsId; }

static const char vcid_object_cc[] = "$Header$";
#endif // !OBJECT_CC_DEFINED
