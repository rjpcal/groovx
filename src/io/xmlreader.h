/** @file io/xmlreader.h io::reader implementation for reading XML
    files in the GVX format */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
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

namespace nub
{
  template <class T> class ref;
}

namespace io
{
  class serializable;
  class reader;

  nub::ref<io::serializable> load_gvx(const char* filename);

  void xml_debug(const char* filename);
}

static const char __attribute__((used)) vcid_groovx_io_xmlreader_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_XMLREADER_H_UTC20050626084021_DEFINED
