///////////////////////////////////////////////////////////////////////
//
// ioutil.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 11 21:43:28 1999
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

#ifndef IOUTIL_CC_DEFINED
#define IOUTIL_CC_DEFINED

#include "io/ioutil.h"

#include "io/asciistreamreader.h"
#include "io/asciistreamwriter.h"
#include "io/io.h"
#include "io/iolegacy.h"
#include "io/xmlwriter.h"

#include "util/cstrstream.h"
#include "util/error.h"
#include "util/pointers.h"
#include "util/ref.h"
#include "util/strings.h"

#include <iostream>
#include <sstream>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

fstring IO::writeLGX(Util::Ref<IO::IoObject> obj)
{
  std::ostringstream ost;

  IO::LegacyWriter writer(ost);
  writer.writeRoot(obj.get());

  return fstring(ost.str().c_str());
}

void IO::readLGX(Util::Ref<IO::IoObject> obj, const char* buf)
{
  rutz::icstrstream ist(buf);

  IO::LegacyReader reader(ist);
  reader.readRoot(obj.get());
}

fstring IO::writeASW(Util::Ref<IO::IoObject> obj)
{
  std::ostringstream ost;

  shared_ptr<IO::Writer> writer = IO::makeAsciiStreamWriter(ost);
  writer->writeRoot(obj.get());

  return fstring(ost.str().c_str());
}

void IO::readASW(Util::Ref<IO::IoObject> obj, const char* buf)
{
  rutz::icstrstream ist(buf);

  shared_ptr<IO::Reader> reader = IO::makeAsciiStreamReader(ist);
  reader->readRoot(obj.get());
}

fstring IO::writeGVX(Util::Ref<IO::IoObject> obj)
{
  std::ostringstream ost;

  shared_ptr<IO::Writer> writer = IO::makeXMLWriter(ost);
  writer->writeRoot(obj.get());

  return fstring(ost.str().c_str());
}

void IO::saveASW(Util::Ref<IO::IoObject> obj, fstring fname)
{
  shared_ptr<IO::Writer> writer = IO::makeAsciiStreamWriter(fname.c_str());
  writer->writeRoot(obj.get());
}

void IO::loadASW(Util::Ref<IO::IoObject> obj, fstring fname)
{
  shared_ptr<IO::Reader> reader = IO::makeAsciiStreamReader(fname.c_str());
  reader->readRoot(obj.get());
}

void IO::saveGVX(Util::Ref<IO::IoObject> obj, fstring filename)
{
  shared_ptr<IO::Writer> writer = IO::makeXMLWriter(filename.c_str());
  writer->writeRoot(obj.get());
}

Util::Ref<IO::IoObject> IO::retrieveASW(fstring fname)
{
  shared_ptr<IO::Reader> reader = IO::makeAsciiStreamReader(fname.c_str());
  return reader->readRoot();
}

static const char vcid_ioutil_cc[] = "$Header$";
#endif // !IOUTIL_CC_DEFINED
