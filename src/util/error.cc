///////////////////////////////////////////////////////////////////////
//
// error.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jun 22 14:59:48 1999
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

#ifndef ERROR_CC_DEFINED
#define ERROR_CC_DEFINED

#include "util/error.h"

#include "util/strings.h"

#include <cstdlib>
#include <new>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
  static Util::BackTrace* last = 0;
}

Util::Error::Error() :
  std::exception(),
  itsInfo(),
  itsBackTrace(new BackTrace(Util::BackTrace::current()))
{
DOTRACE("Util::Error::Error()");

  dbgDump(4, itsInfo);

  if (last == 0)
    last = new BackTrace(*itsBackTrace);
  else
    *last = *itsBackTrace;

  if (GET_DBG_LEVEL() >= 4)
    {
      itsBackTrace->print();
    }
}

Util::Error::Error(const fstring& msg) :
  std::exception(),
  itsInfo(msg),
  itsBackTrace(new BackTrace(Util::BackTrace::current()))
{
DOTRACE("Util::Error::Error(fstring)");

  dbgDump(4, itsInfo);

  if (last == 0)
    last = new BackTrace(*itsBackTrace);
  else
    *last = *itsBackTrace;

  if (GET_DBG_LEVEL() >= 4)
    {
      itsBackTrace->print();
    }
}

Util::Error::Error(const Util::Error& other) throw() :
  std::exception(other),
  itsInfo(other.itsInfo),
  itsBackTrace(0)
{
DOTRACE("Util::Error::Error(copy)");

  dbgDump(4, itsInfo);

  if (other.itsBackTrace != 0)
    itsBackTrace = new (std::nothrow) BackTrace(*other.itsBackTrace);
}

Util::Error::~Error() throw()
{
DOTRACE("Util::Error::~Error");
  delete itsBackTrace;
}

const char* Util::Error::what() const throw()
{
  return itsInfo.c_str();
}

const Util::BackTrace& Util::Error::lastBackTrace()
{
  if (last == 0)
    last = new BackTrace();

  return *last;
}

static const char vcid_error_cc[] = "$Header$";
#endif // !ERROR_CC_DEFINED
