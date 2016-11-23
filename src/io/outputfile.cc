/** @file io/outputfile.cc wraps a std::ofstream in an io::serializable,
    allowing named files to be serialized and restored across multiple
    program runs */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri May 23 10:03:19 2003
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

#include "outputfile.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/error.h"
#include "rutz/sfmt.h"

#include <fstream>

#include "rutz/trace.h"

using std::unique_ptr;

output_file::output_file() :
  m_filename(""),
  m_stream()
{
}

output_file::~output_file() noexcept {}

void output_file::read_from(io::reader& reader)
{
  rutz::fstring fname;
  reader.read_value("filename", fname);

  set_filename(fname);
}

void output_file::write_to(io::writer& writer) const
{
  writer.write_value("filename", m_filename);
}

rutz::fstring output_file::get_filename() const
{
  return m_filename;
}

void output_file::set_filename(rutz::fstring fname)
{
  if (fname.is_empty())
    {
      m_stream.reset();
      return;
    }

  unique_ptr<std::ostream> s(std::make_unique<std::ofstream>(fname.c_str()));

  if (s->fail())
    throw rutz::error(rutz::sfmt("couldn't open '%s' for writing",
                                 fname.c_str()), SRC_POS);

  m_stream.swap(s);
  m_filename = fname;
}

bool output_file::has_stream() const
{
  return (m_stream.get() != nullptr && !m_stream->fail());
}

std::ostream& output_file::stream()
{
  if (!has_stream())
    throw rutz::error("output_file object's stream is invalid", SRC_POS);

  return *m_stream;
}
