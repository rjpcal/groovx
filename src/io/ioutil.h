/** @file io/ioutil.h wrapper functions around the various available
    read/write formats */

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

namespace io
{
  class serializable;

  // LGX -- "legacy" compact format
  rutz::fstring  write_lgx(nub::ref<io::serializable> obj);
  void           read_lgx(nub::ref<io::serializable> obj, const char* buf);

  // ASW -- "ascii stream writer" format
  rutz::fstring  write_asw(nub::ref<io::serializable> obj);
  void           read_asw(nub::ref<io::serializable> obj, const char* buf);

  void save_asw(nub::ref<io::serializable> obj, rutz::fstring filename);
  void load_asw(nub::ref<io::serializable> obj, rutz::fstring filename);

  nub::ref<io::serializable> retrieve_asw(rutz::fstring filename);

  // GVX -- GroovX XML format
  rutz::fstring  write_gvx(nub::ref<io::serializable> obj);

  void save_gvx(nub::ref<io::serializable> obj, rutz::fstring filename);
}

static const char vcid_groovx_io_ioutil_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_IOUTIL_H_UTC20050626084021_DEFINED
