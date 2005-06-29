///////////////////////////////////////////////////////////////////////
//
// xmlreader.h
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jun 20 16:09:38 2003
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_IO_XMLREADER_H_UTC20050626084021_DEFINED
#define GROOVX_IO_XMLREADER_H_UTC20050626084021_DEFINED

namespace Nub
{
  template <class T> class Ref;
}

namespace IO
{
  class IoObject;
  class Reader;

  Nub::Ref<IO::IoObject> loadGVX(const char* filename);

  void xmlDebug(const char* filename);
}

static const char vcid_groovx_io_xmlreader_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_XMLREADER_H_UTC20050626084021_DEFINED
