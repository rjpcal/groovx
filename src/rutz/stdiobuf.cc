/** @file rutz/stdiobuf.cc wrap posix file descriptors in c++ iostreams */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Feb 25 13:52:11 2003
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

#include "rutz/stdiobuf.h"

#include "rutz/error.h"

#include <ios>
#include <unistd.h>

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

rutz::stdiobuf::stdiobuf(int fd, std::ios::openmode om, bool throw_exception) :
  rutz::stream_buffer(om), m_filedes(fd)
{
GVX_TRACE("rutz::stdiobuf::stdiobuf(int)");

  if (throw_exception && !is_open())
    {
      throw rutz::error("couldn't open file stdiobuf", SRC_POS);
    }
}

void rutz::stdiobuf::close()
{
GVX_TRACE("rutz::stdiobuf::close");
  if (is_open())
    {
      this->sync();
      // NOTE: We don't do ::close(m_filedes) here since we leave that
      // up to whoever instantiated this stdiobuf.
      m_filedes = -1;
    }
}

ssize_t rutz::stdiobuf::do_read(char* mem, size_t n)
{
  if (m_filedes < 0) return EOF;
  return ::read(m_filedes, mem, n);
}

ssize_t rutz::stdiobuf::do_write(const char* mem, size_t n)
{
  if (m_filedes < 0) return EOF;
  return ::write(m_filedes, mem, n);
}
