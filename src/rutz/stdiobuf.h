/** @file rutz/stdiobuf.h wrap posix file descriptors in c++ iostreams */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Feb 25 13:27:36 2003
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

#ifndef GROOVX_RUTZ_STDIOBUF_H_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_STDIOBUF_H_UTC20050626084019_DEFINED

#include "rutz/stream_buffer.h"

#include <streambuf>
#include <istream>

namespace rutz
{
  class stdiobuf;
  class stdiostream;
}

/// A C++ streambuf that wraps a standard posix file descriptor.
class rutz::stdiobuf : public rutz::stream_buffer
{
private:
  int m_filedes;

public:

  /// Create with a reference to a file descriptor.
  /** The stdiobuf will NOT close the descriptor on destruction, that
      is left up to the caller (since the caller must also have been
      the one to open the file descriptor). */
  stdiobuf(int fd, std::ios::openmode om, bool throw_exception=false);

  /// Destructor flushes buffer, but DOES NOT CLOSE the file descriptor.
  ~stdiobuf() { close(); }

  stdiobuf(const stdiobuf&) = delete;
  stdiobuf& operator=(const stdiobuf&) = delete;

  /// Check whether we have an open file descriptor.
  bool is_open() const { return m_filedes >= 0; }

  /// Flushes the buffer and forgets the file descriptor, but DOESN'T CLOSE IT.
  /** It is assumed that the same caller who passed the open file
      descriptor to our constructor will also eventually close that
      file descriptor. */
  void close();

  /// Read from the underlying file descriptor
  virtual ssize_t do_read(char* mem, size_t n) override;

  /// Write to the underlying file descriptor
  virtual ssize_t do_write(const char* mem, size_t n) override;
};

class rutz::stdiostream : public std::iostream
{
private:
  rutz::stdiobuf m_buf;

public:
  /// Create with a reference to a file descriptor.
  /** The stdiobuf will NOT close the descriptor on destruction, that
      is left up to the caller (since the caller must also have been
      the one to open the file descriptor). */
  stdiostream(int fd, unsigned int om, bool throw_exception=false) :
    std::iostream(nullptr),
    m_buf(fd, om, throw_exception)
  {
    rdbuf(&m_buf);
  }

  /// Check whether we have an open file descriptor.
  bool is_open() const { return m_buf.is_open(); }

  /// Flushes the buffer and forgets the file descriptor, but DOESN'T CLOSE IT.
  void close() { m_buf.close(); }
};

#endif // !GROOVX_RUTZ_STDIOBUF_H_UTC20050626084019_DEFINED
