///////////////////////////////////////////////////////////////////////
//
// readutils.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Nov 16 14:25:40 1999
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

#ifndef READUTILS_CC_DEFINED
#define READUTILS_CC_DEFINED

#include "io/readutils.h"

#include "util/fstring.h"

using rutz::fstring;

fstring IO::ReadUtils::makeElementNameString(const fstring& seq_name,
                                             int element_num)
{
  fstring result(seq_name);
  result.append( element_num );
  return result;
}

fstring IO::ReadUtils::makeSeqCountString(const fstring& seq_name)
{
  fstring result(seq_name);
  result.append( "Count" );
  return result;
}

static const char vcid_readutils_cc[] = "$Id$ $URL$";
#endif // !READUTILS_CC_DEFINED
