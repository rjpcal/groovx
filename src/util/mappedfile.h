///////////////////////////////////////////////////////////////////////
//
// mappedfile.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Oct  8 13:50:14 2004
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

#ifndef MAPPEDFILE_H_DEFINED
#define MAPPEDFILE_H_DEFINED

#include <sys/stat.h>  // for stat()

namespace rutz
{
  class mapped_file
  {
  public:
    mapped_file(const char* filename);
    ~mapped_file();

    const void* memory() const { return m_mem; }

    off_t length() const { return m_statbuf.st_size; }

    time_t mtime() const { return m_statbuf.st_mtime; }

  private:
    mapped_file(const mapped_file&);
    mapped_file& operator=(const mapped_file&);

    struct stat m_statbuf;
    int         m_fileno;
    void*       m_mem;
  };
}

static const char vcid_mappedfile_h[] = "$Id$ $URL$";
#endif // !MAPPEDFILE_H_DEFINED
