/** @file io/ioutil.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jun 11 21:43:43 1999
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

#ifndef GROOVX_IO_IOUTIL_H_UTC20050626084021_DEFINED
#define GROOVX_IO_IOUTIL_H_UTC20050626084021_DEFINED

namespace rutz
{
  class fstring;
}

namespace nub
{
  template <class T> class ref;
}

namespace IO
{
  class IoObject;

  // LGX -- "Legacy" compact format
  rutz::fstring  writeLGX(nub::ref<IO::IoObject> obj);
  void           readLGX(nub::ref<IO::IoObject> obj, const char* buf);

  // ASW -- AsciiStreamWriter format
  rutz::fstring  writeASW(nub::ref<IO::IoObject> obj);
  void           readASW(nub::ref<IO::IoObject> obj, const char* buf);

  void saveASW(nub::ref<IO::IoObject> obj, rutz::fstring filename);
  void loadASW(nub::ref<IO::IoObject> obj, rutz::fstring filename);

  nub::ref<IO::IoObject> retrieveASW(rutz::fstring filename);

  // GVX -- GroovX XML format
  rutz::fstring  writeGVX(nub::ref<IO::IoObject> obj);

  void saveGVX(nub::ref<IO::IoObject> obj, rutz::fstring filename);
}

static const char vcid_groovx_io_ioutil_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_IOUTIL_H_UTC20050626084021_DEFINED
