///////////////////////////////////////////////////////////////////////
//
// error.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jun 22 14:59:48 1999
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

#ifndef ERROR_CC_DEFINED
#define ERROR_CC_DEFINED

#include "util/error.h"

#include "util/strings.h"

#include <cstdlib>

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  static Util::BackTrace* last = 0;
}

Util::Error::Error() :
  itsInfo(),
  itsBackTrace(new BackTrace(Util::BackTrace::current()))
{
DOTRACE("Util::Error::Error()");

  dbgDump(4, itsInfo);

  if (last == 0)
    last = new BackTrace(*itsBackTrace);
  else
    *last = *itsBackTrace;
}

Util::Error::Error(const fstring& msg) :
  itsInfo(msg),
  itsBackTrace(new BackTrace(Util::BackTrace::current()))
{
DOTRACE("Util::Error::Error(fstring)");

  dbgDump(4, itsInfo);

  if (last == 0)
    last = new BackTrace(*itsBackTrace);
  else
    *last = *itsBackTrace;
}

Util::Error::Error(const Util::Error& other) :
  itsInfo(other.itsInfo),
  itsBackTrace(new BackTrace(*other.itsBackTrace))
{
DOTRACE("Util::Error::Error(copy)");

  dbgDump(4, itsInfo);
}

Util::Error::~Error() throw()
{
DOTRACE("Util::Error::~Error");
  delete itsBackTrace;
}

const Util::BackTrace& Util::Error::lastBackTrace()
{
  if (last == 0)
    last = new BackTrace();

  return *last;
}

static const char vcid_error_cc[] = "$Header$";
#endif // !ERROR_CC_DEFINED
