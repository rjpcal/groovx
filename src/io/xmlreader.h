///////////////////////////////////////////////////////////////////////
//
// xmlreader.h
//
// Copyright (c) 2003-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 20 16:09:38 2003
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

#ifndef XMLREADER_H_DEFINED
#define XMLREADER_H_DEFINED

#include "util/pointers.h"

#ifdef HAVE_IOSFWD
#  include <iosfwd>
#else
class istream;
#endif

namespace IO
{
  class Reader;

  /// Make an XML reader that reads from \c STD_IO::istream.
  shared_ptr<IO::Reader> makeXMLReader(STD_IO::istream& is);

  void xmlDebug(const char* filename);
}

static const char vcid_xmlreader_h[] = "$Header$";
#endif // !XMLREADER_H_DEFINED
