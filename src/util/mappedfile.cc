///////////////////////////////////////////////////////////////////////
//
// mappedfile.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Oct  8 14:11:31 2004
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

#ifndef MAPPEDFILE_CC_DEFINED
#define MAPPEDFILE_CC_DEFINED

#include "mappedfile.h"

#include "util/error.h"
#include "util/strings.h"

#include <cerrno>
#include <cstring>     // for strerror()
#include <fcntl.h>     // for open(), O_RDONLY
#include <sys/mman.h>  // for mmap()
#include <unistd.h>    // for close()

rutz::mapped_file::mapped_file(const char* filename)
  :
  m_statbuf(),
  m_fileno(0),
  m_mem(0)
{
  errno = 0;

  if (stat(filename, &m_statbuf) == -1)
    {
      fstring msg("stat() failed for file ", filename, ":\n",
                  strerror(errno), "\n");
      throw rutz::error(msg, SRC_POS);
    }

  m_fileno = open(filename, O_RDONLY);
  if (m_fileno == -1)
    {
      fstring msg("open() failed for file ", filename, ":\n",
                  strerror(errno), "\n");
      throw rutz::error(msg, SRC_POS);
    }

  m_mem = mmap(0, m_statbuf.st_size,
               PROT_READ, MAP_PRIVATE, m_fileno, 0);

  if (m_mem == (void*)-1)
    {
      fstring msg("mmap() failed for file ", filename, ":\n",
                  strerror(errno), "\n");
      throw rutz::error(msg, SRC_POS);
    }
}

rutz::mapped_file::~mapped_file()
{
  munmap(m_mem, m_statbuf.st_size);
  close(m_fileno);
}

static const char vcid_mappedfile_cc[] = "$Header$";
#endif // !MAPPEDFILE_CC_DEFINED
