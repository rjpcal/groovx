///////////////////////////////////////////////////////////////////////
//
// outputfile.cc
//
// Copyright (c) 2003-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri May 23 10:03:19 2003
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

#ifndef OUTPUTFILE_CC_DEFINED
#define OUTPUTFILE_CC_DEFINED

#include "outputfile.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/error.h"

#include <fstream>

#include "util/trace.h"

OutputFile::OutputFile() :
  itsFilename(""),
  itsStream(0)
{
}

OutputFile::~OutputFile() throw() {}

void OutputFile::readFrom(IO::Reader& reader)
{
  fstring fname;
  reader.readValue("filename", fname);

  setFilename(fname);
}

void OutputFile::writeTo(IO::Writer& writer) const
{
  writer.writeValue("filename", itsFilename);
}

fstring OutputFile::getFilename() const
{
  return itsFilename;
}

void OutputFile::setFilename(fstring fname)
{
  if (fname.is_empty())
    {
      itsStream.reset(0);
      return;
    }

  shared_ptr<STD_IO::ostream> s(new STD_IO::ofstream(fname.c_str()));

  if (s->fail())
    throw Util::Error(fstring("couldn't open '", fname, "' for writing"));

  itsStream.swap(s);
  itsFilename = fname;
}

bool OutputFile::hasStream() const
{
  return (itsStream.get() != 0 && !itsStream->fail());
}

STD_IO::ostream& OutputFile::stream()
{
  if (!hasStream())
    throw Util::Error("OutputFile object's stream is invalid");

  return *itsStream;
}

static const char vcid_outputfile_cc[] = "$Header$";
#endif // !OUTPUTFILE_CC_DEFINED
