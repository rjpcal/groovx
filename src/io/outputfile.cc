/** @file io/outputfile.cc wraps a std::ofstream in an IO::IoObject,
    allowing named files to be serialized and restored across multiple
    program runs */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri May 23 10:03:19 2003
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

#ifndef GROOVX_IO_OUTPUTFILE_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_OUTPUTFILE_CC_UTC20050626084021_DEFINED

#include "outputfile.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/error.h"

#include <fstream>

#include "rutz/trace.h"

using rutz::shared_ptr;

OutputFile::OutputFile() :
  itsFilename(""),
  itsStream()
{
}

OutputFile::~OutputFile() throw() {}

void OutputFile::readFrom(IO::Reader& reader)
{
  rutz::fstring fname;
  reader.readValue("filename", fname);

  setFilename(fname);
}

void OutputFile::writeTo(IO::Writer& writer) const
{
  writer.writeValue("filename", itsFilename);
}

rutz::fstring OutputFile::getFilename() const
{
  return itsFilename;
}

void OutputFile::setFilename(rutz::fstring fname)
{
  if (fname.is_empty())
    {
      itsStream.reset();
      return;
    }

  shared_ptr<std::ostream> s(new std::ofstream(fname.c_str()));

  if (s->fail())
    throw rutz::error(rutz::fstring("couldn't open '", fname,
                                    "' for writing"), SRC_POS);

  itsStream.swap(s);
  itsFilename = fname;
}

bool OutputFile::hasStream() const
{
  return (itsStream.get() != 0 && !itsStream->fail());
}

std::ostream& OutputFile::stream()
{
  if (!hasStream())
    throw rutz::error("OutputFile object's stream is invalid", SRC_POS);

  return *itsStream;
}

static const char vcid_groovx_io_outputfile_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_OUTPUTFILE_CC_UTC20050626084021_DEFINED
