///////////////////////////////////////////////////////////////////////
//
// writeutils.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Nov 16 14:21:32 1999
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

#ifndef WRITEUTILS_CC_DEFINED
#define WRITEUTILS_CC_DEFINED

#include "io/writeutils.h"

#include "io/readutils.h"

fstring IO::WriteUtils::makeElementNameString(const fstring& seq_name,
                                              int element_num)
{
  return ReadUtils::makeElementNameString(seq_name, element_num);
}

fstring IO::WriteUtils::makeSeqCountString(const fstring& seq_name)
{
  return ReadUtils::makeSeqCountString(seq_name);
}

static const char vcid_writeutils_cc[] = "$Header$";
#endif // !WRITEUTILS_CC_DEFINED
