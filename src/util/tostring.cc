///////////////////////////////////////////////////////////////////////
//
// tostring.cc
//
// Copyright (c) 2001-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sun Aug  5 20:12:30 2001
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

#ifndef TOSTRING_CC_DEFINED
#define TOSTRING_CC_DEFINED

#include "util/tostring.h"

#include <cstdio>
#include <cstring>

namespace
{
  const int SZ = 64;
  static char buf[SZ];
}

Util::CharData::CharData(const char* t) :
  text(t), len(t ? strlen(t) : 0)
{}

const char* Util::num2str(bool x)         { snprintf(buf, SZ, "%d", int(x)); return buf; }
const char* Util::num2str(int x)          { snprintf(buf, SZ, "%d", x); return buf; }
const char* Util::num2str(unsigned int x) { snprintf(buf, SZ, "%u", x); return buf; }
const char* Util::num2str(long x)         { snprintf(buf, SZ, "%ld", x); return buf; }
const char* Util::num2str(unsigned long x){ snprintf(buf, SZ, "%lu", x); return buf; }
const char* Util::num2str(double x)       { snprintf(buf, SZ, "%g", x); return buf; }

static const char vcid_tostring_cc[] = "$Header$";
#endif // !TOSTRING_CC_DEFINED
