/** @file rutz/compat_snprintf.h Compatibility declarations for snprintf() and vsnprintf() */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jan 16 17:43:41 2006
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

#ifndef GROOVX_RUTZ_COMPAT_SNPRINTF_H_UTC20060117014341_DEFINED
#define GROOVX_RUTZ_COMPAT_SNPRINTF_H_UTC20060117014341_DEFINED

#ifdef MISSING_SNPRINTF
extern "C"
{
  int snprintf(char*, size_t, const char* format, ...);

  int vsnprintf(char*, size_t, const char* format, __builtin_va_list ap);
}
#else
#include <cstdio>
#endif

static const char vcid_groovx_rutz_compat_snprintf_h_utc20060117014341[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_COMPAT_SNPRINTF_H_UTC20060117014341DEFINED
