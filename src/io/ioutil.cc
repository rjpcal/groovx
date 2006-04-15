/** @file io/ioutil.cc wrapper functions around the various available
    read/write formats */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jun 11 21:43:28 1999
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

#ifndef GROOVX_IO_IOUTIL_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_IOUTIL_CC_UTC20050626084021_DEFINED

#include "io/ioutil.h"

#include "io/asciistreamreader.h"
#include "io/asciistreamwriter.h"
#include "io/io.h"
#include "io/iolegacy.h"
#include "io/xmlwriter.h"

#include "nub/ref.h"

#include "rutz/cstrstream.h"
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sharedptr.h"

#include <iostream>
#include <sstream>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;
using rutz::shared_ptr;

fstring io::write_lgx(nub::ref<io::serializable> obj)
{
  std::ostringstream ost;

  io::legacy_writer writer(ost);
  writer.write_root(obj.get());

  return fstring(ost.str().c_str());
}

void io::read_lgx(nub::ref<io::serializable> obj, const char* buf)
{
  rutz::icstrstream ist(buf);

  io::legacy_reader reader(ist);
  reader.read_root(obj.get());
}

fstring io::write_asw(nub::ref<io::serializable> obj)
{
  std::ostringstream ost;

  shared_ptr<io::writer> writer = io::make_asw_writer(ost);
  writer->write_root(obj.get());

  return fstring(ost.str().c_str());
}

void io::read_asw(nub::ref<io::serializable> obj, const char* buf)
{
  rutz::icstrstream ist(buf);

  shared_ptr<io::reader> reader = io::make_asw_reader(ist);
  reader->read_root(obj.get());
}

fstring io::write_gvx(nub::ref<io::serializable> obj)
{
  std::ostringstream ost;

  shared_ptr<io::writer> writer = io::make_xml_writer(ost);
  writer->write_root(obj.get());

  return fstring(ost.str().c_str());
}

void io::save_asw(nub::ref<io::serializable> obj, fstring fname)
{
  shared_ptr<io::writer> writer = io::make_asw_writer(fname.c_str());
  writer->write_root(obj.get());
}

void io::load_asw(nub::ref<io::serializable> obj, fstring fname)
{
  shared_ptr<io::reader> reader = io::make_asw_reader(fname.c_str());
  reader->read_root(obj.get());
}

void io::save_gvx(nub::ref<io::serializable> obj, fstring filename)
{
  shared_ptr<io::writer> writer = io::make_xml_writer(filename.c_str());
  writer->write_root(obj.get());
}

nub::ref<io::serializable> io::retrieve_asw(fstring fname)
{
  shared_ptr<io::reader> reader = io::make_asw_reader(fname.c_str());
  return reader->read_root();
}

static const char __attribute__((used)) vcid_groovx_io_ioutil_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_IOUTIL_CC_UTC20050626084021_DEFINED
